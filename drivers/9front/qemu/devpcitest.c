/* QEMU PCI Test Device */

#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../port/pci.h"
#include "../port/error.h"
#include "io.h"

enum {
	Qctl = 1,
};

#define TYPE(x) ((x)&3)
#define ID(x) ((x) >> 2)
#define QID(i, t) (((i) << 2) | (t))

typedef struct Hdr Hdr;
typedef struct Ptd Ptd;

struct Hdr {
	u8int test;
	u8int width;
	u8int pad0[2];
	u32int offset;
	u32int data;
	u32int count;
	u8int name[64];
};

struct Ptd {
	QLock;
	Pcidev *pdev;
	u8int *mmio[4];
	int io[4];
	int active;
};

static Ptd *ptd;
static ulong nptd;
static Dirtab *ptddir;

void
csr8w(Ptd *t, int i, ulong off, ulong val)
{
	if (i >= nelem(t->mmio) || t->pdev->mem[i].size < sizeof(Hdr))
		error(Ebadarg);

	if (t->pdev->mem[i].bar & 0x1)
		outb(t->io[i] + off, val);
	else
		t->mmio[i][off] = val;
}

u8int
csr8r(Ptd *t, int i, ulong off)
{
	if (t->pdev->mem[i].bar & 0x1)
		return inb(t->io[i] + off);
	else
		return t->mmio[i][off];
}

u32int
csr32r(Ptd *t, int i, ulong off)
{
	if (t->pdev->mem[i].bar & 0x1)
		return inl(t->io[i] + off);
	else
		return *((u32int *)&t->mmio[i][off]);
}

static Chan *
ptdattach(char *spec)
{
	if (!iseve())
		error(Eperm);
	return devattach('T', spec);
}

static Walkqid *
ptdwalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, ptddir, nptd + 1, devgen);
}

static int
ptdstat(Chan *c, uchar *dp, int n)
{
	return devstat(c, dp, n, ptddir, nptd + 1, devgen);
}

static Chan *
ptdopen(Chan *c, int omode)
{
	return devopen(c, omode, ptddir, nptd + 1, devgen);
}

static void
ptdclose(Chan *)
{
}

static int
readhdr(Ptd *t, int i, Hdr *h)
{
	int j;

	if (t->pdev->mem[i].size < sizeof(Hdr))
		return 0;

	h->test = csr8r(t, i, 0x0);
	h->width = csr8r(t, i, 0x1);
	h->offset = csr32r(t, i, 0x4);
	h->data = csr32r(t, i, 0x8);
	h->count = csr32r(t, i, 0xc);
	for (j = 0; j < nelem(h->name); j++)
		h->name[j] = csr8r(t, i, j + 0x10);
	h->name[nelem(h->name) - 1] = '\0';
	return 1;
}

static long
ptdread(Chan *c, void *buf, long n, vlong off)
{
	Ptd *t;
	Hdr h;
	char *p;
	long nr;
	int i, l;

	if (c->qid.type & QTDIR)
		return devdirread(c, buf, n, ptddir, nptd + 1, devgen);

	t = ptd + ID(c->qid.path);
	if (!t->active)
		error(Eio);

	qlock(t);
	if (waserror()) {
		qunlock(t);
		nexterror();
	}

	nr = 0;
	switch (TYPE(c->qid.path)) {
	case Qctl:
		p = smalloc(READSTR);
		l = 0;
		for (i = 0; i < nelem(t->mmio); i++) {
			if (!readhdr(t, i, &h))
				continue;
			l += snprint(p + l, READSTR - l, "header %d\n", i);
			l += snprint(p + l, READSTR - l, "test   %d\n", h.test);
			l += snprint(p + l, READSTR - l, "width  %d\n", h.width);
			l += snprint(p + l, READSTR - l, "offset %d\n", h.offset);
			l += snprint(p + l, READSTR - l, "data   %d\n", h.data);
			l += snprint(p + l, READSTR - l, "count  %d\n", h.count);
			l += snprint(p + l, READSTR - l, "name   %s\n", h.name);
			l += snprint(p + l, READSTR - l, "\n");
		}
		nr = readstr(off, buf, n, p);
		free(p);
		break;
	}
	qunlock(t);
	poperror();

	return nr;
}

static ulong
xatoul(char *s)
{
	char *rptr;
	uvlong n;

	n = strtoul(s, &rptr, 0);
	if (*rptr != '\0')
		error(Ebadarg);
	return n;
}

static long
ptdwrite(Chan *c, void *buf, long n, vlong off)
{
	Ptd *t;
	char p[64], *a[2];
	long wn;

	if (c->qid.type & QTDIR)
		error(Eperm);

	t = ptd + ID(c->qid.path);
	if (!t->active)
		error(Eio);

	qlock(t);
	if (waserror()) {
		qunlock(t);
		nexterror();
	}

	wn = 0;
	switch (TYPE(c->qid.path)) {
	case Qctl:
		if (n >= sizeof(p))
			error(Etoobig);
		memset(a, 0, sizeof(a));
		memmove(p, buf, n);
		p[n] = '\0';
		tokenize(p, a, nelem(a));
		if (a[0] == nil || a[1] == nil)
			error(Ebadarg);
		csr8w(t, xatoul(a[0]), 0x0, xatoul(a[1]));
		break;
	}

	qunlock(t);
	poperror();
	return wn;
}

static void
ptdprobe(void)
{
	Pcidev *p;
	Ptd *t;
	Dirtab *dp;
	ulong i, j, n;

	n = 0;
	p = nil;
	while ((p = pcimatch(p, 0x1b36, 0x0005)) && p->ccrb == 0)
		n++;
	if (n == 0)
		return;

	ptd = malloc(sizeof(*ptd) * n);
	ptddir = malloc(sizeof(*ptddir) * (n + 1));
	if (ptd == nil || ptddir == nil) {
		print("ptd: failed to allocate memory for Ptds\n");
		free(ptd);
		free(ptddir);
		return;
	}
	nptd = n;

	i = 0;
	p = nil;
	while ((p = pcimatch(p, 0x1b36, 0x0005)) && p->ccrb == 0) {
		if (i >= n)
			break;
		t = ptd + i;
		t->pdev = p;
		t->active = 1;
		for (j = 0; j < nelem(t->mmio); j++) {
			if (p->mem[j].size < sizeof(Hdr))
				continue;

			if (p->mem[j].bar & 0x1) {
				t->io[j] = p->mem[j].bar & ~0xf;
				if (ioalloc(t->io[j], p->mem[j].size, 0, "ptd") < 0) {
					print("ptd: device %lud failed to allocate io size %lud\n", j, p->mem[j].size);
					t->active = 0;
				}
			} else {
				t->mmio[j] = vmap(p->mem[j].bar & ~0xf, p->mem[j].size);
				if (t->mmio[j] == nil) {
					print("ptd: device %lud failed to allocate mmio size %lud\n", j, p->mem[j].size);
					t->active = 0;
				}
			}
		}
		i++;
	}

	dp = ptddir;
	strcpy(dp->name, ".");
	mkqid(&dp->qid, 0, 0, QTDIR);
	dp->length = 0;
	dp->perm = DMDIR | 0555;
	dp++;
	for (i = 0; i < nptd; i++) {
		snprint(dp->name, sizeof(dp->name), "ptd%ludctl", i);
		dp->qid.path = QID(i, Qctl);
		dp->perm = 0666;
		dp++;
	}
}

static void
ptdreset(void)
{
	ptdprobe();
}

Dev pcitestdevtab = {
    'T',
    "pcitest",

    ptdreset,
    devinit,
    devshutdown,
    ptdattach,
    ptdwalk,
    ptdstat,
    ptdopen,
    devcreate,
    ptdclose,
    ptdread,
    devbread,
    ptdwrite,
    devbwrite,
    devremove,
    devwstat,
    devpower,
};
