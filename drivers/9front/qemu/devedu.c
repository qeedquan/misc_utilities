/* QEMU EDU Misc Device */

#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../port/error.h"
#include "io.h"

enum
{
	DMAADDR = 0x40000,
	DMASIZE = 0x1000,
};

enum
{
	Qctl = 1,
	Qdata,
};

typedef struct Edu Edu;

struct Edu
{
	QLock;
	Pcidev *pdev;
	u8int  *mmio;
	uchar  *dmabuf;
	int    active;
};

static char Einactive[] = "inactive device";
static char Ebadcmd[]	= "bad command (invalid address/value or unknown args)";

static Edu      *edu;
static ulong    nedu;
static Dirtab   *edudir;
static ulong    nedudir;

#define TYPE(x)		((x)&3)
#define ID(x)		((x)>>2)
#define QID(i, t)	(((i)<<2)|(t))

#define min(a, b) (((a) < (b)) ? (a) : (b))

#define csr32r(c, a)	(*((u32int*)(((c)->mmio)+((a)))))
#define csr32w(c, a, v) (*((u32int*)(((c)->mmio)+((a)))) = (v))
#define csr64r(c, a)	(*((u64int*)(((c)->mmio)+((a)))))
#define csr64w(c, a, v) (*((u64int*)(((c)->mmio)+((a)))) = (v))

static Chan*
eduattach(char *spec)
{
	return devattach('U', spec);
}

static Walkqid*
eduwalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, edudir, nedudir, devgen);
}

static int
edustat(Chan *c, uchar *dp, int n)
{
	return devstat(c, dp, n, edudir, nedudir, devgen);
}

static Chan*
eduopen(Chan *c, int omode)
{
	return devopen(c, omode, edudir, nedudir, devgen);
}

static void
educlose(Chan*)
{
}

static long
eduread(Chan *c, void *buf, long n, vlong off)
{
	Edu *e;
	char *p;
	long nr;
	int l;

	if (c->qid.type & QTDIR)
		return devdirread(c, buf, n, edudir, nedudir, devgen);

	e = edu + ID(c->qid.path);
	if (!e->active)
		error(Einactive);

	qlock(e);
	if (waserror()) {
		qunlock(e);
		nexterror();
	}

	switch (TYPE(c->qid.path)) {
	case Qctl:
		p = smalloc(READSTR);
		l = 0;
		l += snprint(p+l, READSTR-l, "00 - ident %xu\n", csr32r(e, 0x00));
		l += snprint(p+l, READSTR-l, "04 w liveness %ux\n", csr32r(e, 0x04));
		l += snprint(p+l, READSTR-l, "08 w factorial %ud\n", csr32r(e, 0x08));
		l += snprint(p+l, READSTR-l, "20 - status %d\n", csr32r(e, 0x20));
		l += snprint(p+l, READSTR-l, "24 - intr %x\n", csr32r(e, 0x24));
		l += snprint(p+l, READSTR-l, "60 - rintr\n");
		l += snprint(p+l, READSTR-l, "64 - aintr\n");
		l += snprint(p+l, READSTR-l, "80 - dmasrc %llux\n", csr64r(e, 0x80));
		l += snprint(p+l, READSTR-l, "88 - dmadst %llux\n", csr64r(e, 0x88));
		l += snprint(p+l, READSTR-l, "90 - dmacnt %llux\n", csr64r(e, 0x90));
		snprint(p+l, READSTR-l, "98 - dmacmd %llux\n", csr64r(e, 0x98));
		nr = readstr(off, buf, n, p);
		free(p);
		break;

	case Qdata:
		if (off >= DMASIZE) {
			nr = 0;
			break;
		}
		nr = min(n, DMASIZE-off);
		if (nr <= 0)
			break;
		csr64w(e, 0x80, PCIWADDR(e->dmabuf));
		csr64w(e, 0x88, DMAADDR+off);
		csr64w(e, 0x90, nr-1);
		csr64w(e, 0x98, 0x1);
		while (csr64r(e, 0x98) & 0x1)
			tsleep(&up->sleep, return0, nil, 10);
		memmove(buf, e->dmabuf, nr);
		break;
	}

	qunlock(e);
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
		error(Ebadcmd);
	return n;
}

static uvlong
xatoull(char *s)
{
	char *rptr;
	uvlong n;

	n = strtoull(s, &rptr, 0);
	if (*rptr != '\0')
		error(Ebadcmd);
	return n;
}

static long
eduwrite(Chan *c, void *buf, long n, vlong off)
{
	Edu *e;
	char p[64], *a[3];
	u64int x, y;
	long wn;

	if (c->qid.type & QTDIR)
		error(Eperm);

	e = edu + ID(c->qid.path);
	if (!e->active)
		error(Einactive);

	qlock(e);
	if (waserror()) {
		qunlock(e);
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
		if (a[0] == nil || a[1] == nil || a[2] != nil)
			error(Ebadcmd);
		x = xatoul(a[0]);
		y = xatoul(a[1]);
		switch (x) {
		case 0x04:
			csr32w(e, x, y);
			break;
			
		case 0x08:
			csr32w(e, x, y);
			while (csr32r(e, 0x20) & 0x1)
				tsleep(&up->sleep, return0, nil, 10);
			break;

		default:
			error(Ebadcmd);
		}
		break;

	case Qdata:
		if (off >= DMASIZE) {
			wn = 0;
			break;
		}
		wn = min(n, DMASIZE-off);
		if (wn <= 0)
			break;
		memmove(e->dmabuf, buf, wn);
		csr64w(e, 0x80, DMAADDR+off);
		csr64w(e, 0x88, PCIWADDR(e->dmabuf));
		csr64w(e, 0x90, wn-1);
		csr64w(e, 0x98, 0x3);
		while (csr64r(e, 0x98) & 0x1)
			tsleep(&up->sleep, return0, nil, 10);
		break;
	}

	qunlock(e);
	poperror();
	return wn;
}

static void
eduprobe(void)
{
	Pcidev *p;
	Edu *e;
	Dirtab *dp;
	ulong i, n;
	char name[32];

	n = 8;
	edu = malloc(sizeof(Edu) * n);
	if (edu == nil) {
		print("edu: could not allocate Edus\n");
		goto error;
	}

	p = nil;
	i = 0;
	while (p = pcimatch(p, 0, 0)) {
		if (p->vid != 0x1234 || p->did != 0x11e8)
			continue;
		if (p->ccrb != 0)
			continue;

		if (i >= n) {
			n *= 2;
			edu = realloc(edu, sizeof(Edu) * n);
			if (edu == nil) {
				print("edu: failed to realloc Edus\n");
				goto error;
			}
		}

		e = edu + i;
		memset(e, 0, sizeof(*e));
		e->mmio = vmap(p->mem[0].bar & ~0x0f, p->mem[0].size);
		e->dmabuf = mallocalign(DMASIZE, 8, 0, 0);
		e->pdev = p;
		e->active = 1;
		if (e->mmio == nil || e->dmabuf == nil) {
			print("edu: could not allocate memory for device %lud\n", i);
			e->active = 0;
		} else
			snprint(name, sizeof(name), "edu%lud", i);
		i++;
	}

	nedu = i;
	nedudir = 2*nedu + 1;
	edudir = malloc(sizeof(Dirtab) * nedudir);
	if (edudir == nil) {
		print("edu: failed to allocate Dirtab\n");
		goto error;
	}

	dp = edudir;
	strcpy(dp->name, ".");
	mkqid(&dp->qid, 0, 0, QTDIR);
	dp->length = 0;
	dp->perm = DMDIR|0555;
	dp++;
	for (i = 0; i < nedu; i++) {
		snprint(dp->name, sizeof(dp->name), "edu%ludctl", i);
		dp->qid.path = QID(i, Qctl);
		dp->perm = 0666;
		dp++;

		snprint(dp->name, sizeof(dp->name), "edu%luddata", i);
		dp->qid.path = QID(i, Qdata);
		dp->perm = 0666;
		dp++;
	}

	return;

error:
	free(edu);
	free(edudir);
	nedu = 0;
	nedudir = 0;
}

static void
edureset(void)
{
	eduprobe();
}

Dev edudevtab = {
	'U',
	"edu",

	edureset,
	devinit,
	devshutdown,
	eduattach,
	eduwalk,
	edustat,
	eduopen,
	devcreate,
	educlose,
	eduread,
	devbread,
	eduwrite,
	devbwrite,
	devremove,
	devwstat,
	devpower,
};

