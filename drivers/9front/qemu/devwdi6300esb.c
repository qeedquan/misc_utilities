#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../port/error.h"
#include "io.h"

#define SLOT(c) ((c->qid.path >> 8) & 0xff)
#define TYPE(c) (c->qid.path & 0xff)
#define QID(s, t) (((s) << 8) | (t))

enum {
	Qdir = 0,
	Qctl,
};

typedef struct {
	QLock;
	Pcidev *pdev;
	void *mmio;
	ulong timeout;
	int active;
} Wd;

static Wd *wd;
static ulong nwd;

static uvlong
xatoull(char *s)
{
	char *rptr;
	uvlong n;

	n = strtoull(s, &rptr, 0);
	if (*rptr != '\0')
		error(Ebadarg);
	return n;
}

static void
wdunlock(Wd *w)
{
	u8int *r;

	r = w->mmio;
	r[0xc] = 0x80;
	r[0xc] = 0x86;
}

static void
wdpet(void *)
{
	Wd *w;
	u16int *r;
	ulong i;

	for (;;) {
		tsleep(&up->sleep, return0, 0, 1);
		for (i = 0; i < nwd; i++) {
			w = wd + i;
			if (!w->active)
				continue;

			qlock(w);
			wdunlock(w);
			r = w->mmio;
			r[6] |= 0x100;
			qunlock(w);
		}
	}
}
static Chan *
wdattach(char *spec)
{
	kproc("wdi6300esb", wdpet, nil);
	return devattach('W', spec);
}

static int
wdgen(Chan *c, char *, Dirtab *, int, int i, Dir *dp)
{
	Qid qid;

	if (i == DEVDOTDOT) {
		mkqid(&qid, Qdir, 0, QTDIR);
		devdir(c, qid, "#W", 0, eve, 0555, dp);
		return 1;
	}

	if (i >= nwd)
		return -1;

	snprint(up->genbuf, sizeof(up->genbuf), "wdi6300esb%dctl", i);
	qid.path = QID(i, Qctl);
	qid.vers = 0;
	qid.type = QTFILE;
	devdir(c, qid, up->genbuf, 0, eve, 0660, dp);
	return 1;
}

static Walkqid *
wdwalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, nil, 0, wdgen);
}

static int
wdstat(Chan *c, uchar *db, int n)
{
	return devstat(c, db, n, nil, 0, wdgen);
}

static Chan *
wdopen(Chan *c, int omode)
{
	return devopen(c, omode, nil, 0, wdgen);
}

static void
wdclose(Chan *)
{
}

static long
wdread(Chan *c, void *a, long n, vlong off)
{
	static char *inttab[] = {
	    "irq",
	    "reserved",
	    "smi",
	    "disabled",
	};

	Wd *w;
	char *p;
	long nr;
	int l;
	ulong st, lc;

	if (TYPE(c) == Qdir)
		return devdirread(c, a, n, nil, 0, wdgen);

	w = wd + SLOT(c);
	qlock(w);

	if (waserror()) {
		qunlock(w);
		nexterror();
	}

	if (!w->active)
		error(Eio);

	nr = 0;
	switch (TYPE(c)) {
	case Qctl:
		st = pcicfgr16(w->pdev, 0x60);
		lc = pcicfgr8(w->pdev, 0x68);

		p = smalloc(READSTR);
		l = 0;
		l += snprint(p + l, READSTR - l, "reboot enabled %d\n", !(st & 0x20));
		l += snprint(p + l, READSTR - l, "rate %d\n", (st & 0x4) ? 1000000 : 1000);
		l += snprint(p + l, READSTR - l, "interrupt type %s\n", inttab[st & 0x3]);
		l += snprint(p + l, READSTR - l, "enabled %d\n", !!(lc & 0x2));
		l += snprint(p + l, READSTR - l, "locked %d\n", !!(lc & 0x1));
		l += snprint(p + l, READSTR - l, "timer mode %s\n", (lc & 0x4) ? "freerun" : "reset");
		l += snprint(p + l, READSTR - l, "previous reboot flag %x\n", ((u16int *)w->mmio)[0x6]);
		snprint(p + l, READSTR - l, "timeout %lud (ms)\n", w->timeout);
		nr = readstr(off, a, n, p);
		free(p);
		break;

	default:
		error(Egreg);
		break;
	}

	qunlock(w);
	poperror();
	return nr;
}

static void
wdsettimeout(Wd *w, uvlong timeout)
{
	u32int *r;
	int i;

	if (timeout == 0 || timeout > 0xffffff)
		error(Ebadarg);

	r = w->mmio;
	for (i = 0; i < 2; i++) {
		wdunlock(w);
		r[i] = timeout / 2;
	}
	w->timeout = timeout;
}

static long
wdwrite(Chan *c, void *a, long n, vlong)
{
	Wd *w;
	char p[64], *s, *t[2];
	uvlong tm;

	w = wd + SLOT(c);
	qlock(w);
	if (waserror()) {
		qunlock(w);
		nexterror();
	}

	if (!w->active)
		error(Eio);

	switch (TYPE(c)) {
	case Qdir:
		error(Eperm);
		break;

	case Qctl:
		if (n >= sizeof(p))
			error(Ebadarg);
		memmove(p, a, n);
		p[n] = '\0';
		if (s = strchr(p, '\n'))
			*s = '\0';

		if (strncmp(p, "enable", n) == 0) {
			pcicfgw8(w->pdev, 0x68, pcicfgr8(w->pdev, 0x68) | 0x2);
		} else if (strncmp(p, "disable", n) == 0) {
			pcicfgw8(w->pdev, 0x68, pcicfgr8(w->pdev, 0x68) & ~0x2);
		} else if (strncmp(p, "lock", n) == 0) {
			pcicfgw8(w->pdev, 0x68, pcicfgr8(w->pdev, 0x68) | 0x1);
		} else if (strncmp(p, "timeout", 7) == 0) {
			memset(t, 0, sizeof(t));
			tokenize(p, t, nelem(t));
			if (t[1] == nil)
				error(Ebadarg);

			tm = xatoull(t[1]);
			wdsettimeout(w, tm);
		} else
			error(Ebadarg);

		break;

	default:
		error(Egreg);
		break;
	}

	qunlock(w);
	poperror();

	return 0;
}

static void
wdprobe(void)
{
	Wd *w;
	Pcidev *p;
	ulong i, n;

	n = 0;
	p = nil;
	while (p = pcimatch(p, 0x8086, 0x25ab))
		n++;

	nwd = 0;
	wd = malloc(sizeof(*wd) * n);
	if (wd == nil) {
		print("i6300esb: failed to allocate memory for devices\n");
		return;
	}
	nwd = n;

	p = nil;
	for (i = 0; i < n; i++) {
		w = wd + i;
		p = pcimatch(p, 0x8086, 0x25ab);

		w->active = 1;
		w->mmio = vmap(p->mem[0].bar & ~0xf, p->mem[0].size);
		w->pdev = p;
		if (w->mmio == nil) {
			print("i6300esb: failed to allocate mmio region for device %lud\n", i);
			w->active = 0;
			continue;
		}

		wdsettimeout(w, 1000);
		pcicfgw8(w->pdev, 0x60, pcicfgr8(w->pdev, 0x60) | 0x3);
	}

	print("i6300esb: detected %lud devices\n", n);
}

static void
wdreset(void)
{
	wdprobe();
}

Dev wdi6300esbdevtab = {
    'W',
    "wdi6300esb",

    wdreset,
    devinit,
    devshutdown,
    wdattach,
    wdwalk,
    wdstat,
    wdopen,
    devcreate,
    wdclose,
    wdread,
    devbread,
    wdwrite,
    devbwrite,
    devremove,
    devwstat,
    devpower,
};
