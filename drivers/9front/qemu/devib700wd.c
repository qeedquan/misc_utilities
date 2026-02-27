/* qemu ib700 watchdog timer */
#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../port/error.h"
#include "io.h"

typedef struct Wd Wd;

struct Wd {
	Lock;
	int timeout;
	int state;
	int pet;
};

enum {
	Qdir,
	Qctl,
};

enum {
	STOP = 0x441,
	START = 0x443,

	TICKS = 50,
};

static Dirtab wddir[] = {
    {".", {Qdir, 0, QTDIR}, 0, 0550},
    {"wdctl", {Qctl, 0}, 0, 0660},
};

static Wd wd;

#define t2v(x) (0xf - (((x) + 1) / 2))

static void
wdtimer(void *)
{
	for (;;) {
		tsleep(&up->sleep, return0, 0, TICKS);
		lock(&wd);
		if (wd.state == 1 && wd.pet)
			outb(START, t2v(wd.timeout));
		unlock(&wd);
	}
}

static Chan *
wdattach(char *spec)
{
	if (!iseve())
		error(Eperm);

	if (wd.state >= 0)
		kproc("ib700wd", wdtimer, nil);
	return devattach('T', spec);
}

static Walkqid *
wdwalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, wddir, nelem(wddir), devgen);
}

static int
wdstat(Chan *c, uchar *dp, int n)
{
	return devstat(c, dp, n, wddir, nelem(wddir), devgen);
}

static Chan *
wdopen(Chan *c, int omode)
{
	return devopen(c, omode, wddir, nelem(wddir), devgen);
}

static void
wdclose(Chan *)
{
}

static long
wdread(Chan *c, void *a, long n, vlong off)
{
	char p[64];
	int l, nr;

	lock(&wd);
	if (waserror()) {
		unlock(&wd);
		nexterror();
	}

	nr = 0;
	switch ((int)c->qid.path) {
	case Qdir:
		nr = devdirread(c, a, n, wddir, nelem(wddir), devgen);
		break;

	case Qctl:
		if (wd.state < 0)
			error(Eio);

		l = snprint(p, sizeof(p), "%s (pet: %d)\n", (wd.state == 1) ? "enabled" : "disabled", wd.pet);
		snprint(p + l, sizeof(p) - l, "%ds (1-30)\n", wd.timeout);
		nr = readstr(off, a, n, p);
		break;

	default:
		error(Egreg);
	}

	unlock(&wd);
	poperror();

	return nr;
}

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

static long
wdwrite(Chan *c, void *a, long n, vlong off)
{
	char p[64], *s, *t[2];
	int tm;

	lock(&wd);
	if (waserror()) {
		unlock(&wd);
		nexterror();
	}

	switch ((int)c->qid.path) {
	case Qdir:
		error(Eperm);

	case Qctl:
		if (n >= sizeof(p))
			error(Ebadarg);

		if (wd.state < 0)
			error(Eio);

		memmove(p, a, n);
		p[n] = '\0';
		if (s = strchr(p, '\n'))
			*s = '\0';

		if (strncmp(p, "enable", n) == 0 && wd.state == 0) {
			outb(START, t2v(wd.timeout));
			wd.state = 1;
		} else if (strncmp(p, "disable", n) == 0 && wd.state == 1) {
			outb(STOP, 0);
			wd.state = 0;
		} else if (strncmp(p, "restart", n) == 0) {
			outb(STOP, 0);
			outb(START, t2v(wd.timeout));
			wd.state = 1;
		} else if (strncmp(p, "timeout", 7) == 0) {
			memset(t, 0, sizeof(t));
			tokenize(p, t, nelem(t));
			if (t[1] == nil)
				error(Ebadarg);

			tm = xatoull(t[1]);
			if (tm == 0 || tm > 30)
				error(Ebadarg);
			wd.timeout = tm;
		} else if (strncmp(p, "pet", 3) == 0)
			wd.pet = 1;
		else if (strncmp(p, "nopet", 5) == 0)
			wd.pet = 0;
		else
			error(Ebadarg);
		break;

	default:
		error(Egreg);
	}

	unlock(&wd);
	poperror();
	return 0;
}

static void
wdprobe(void)
{
	wd.timeout = 30;
	wd.state = -1;

	if (ioalloc(STOP, 1, 0, "ib700wdstop") < 0) {
		print("ib700wd: failed to allocate stop port\n");
		return;
	}

	if (ioalloc(START, 1, 0, "ib700wdstart") < 0) {
		print("ib700wd: failed to allocate start port\n");
		iofree(STOP);
		return;
	}

	wd.state = 0;
	wd.pet = 1;
}

static void
wdreset(void)
{
	wdprobe();
}

Dev ib700wddevtab = {
    'T',
    "ib700wd",

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
