#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../port/error.h"
#include "io.h"

typedef struct {
	Lock;
	char buf[sizeof(u32int) * 256];
	ulong len;
} Vcio;

enum {
	Qdir,
	Qvcio,
};

static Dirtab vciodir[] = {
    {".", {Qdir, 0, QTDIR}, 0, 0550},
    {"vcio", {Qvcio, 0}, 0, 0660},
};

static Vcio vcio;

static u32int
leget4(char *p)
{
	return (u32int)p[0] | (u32int)p[1] << 8 | (u32int)p[2] << 16 | (u32int)p[3] << 24;
}

static Chan *
vcioattach(char *spec)
{
	if (!iseve())
		error(Eperm);
	return devattach('V', spec);
}

static Walkqid *
vciowalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, vciodir, nelem(vciodir), devgen);
}

static int
vciostat(Chan *c, uchar *dp, int n)
{
	return devstat(c, dp, n, vciodir, nelem(vciodir), devgen);
}

static Chan *
vcioopen(Chan *c, int omode)
{
	c->aux = smalloc(sizeof(vcio.buf));
	memmove(c->aux, vcio.buf, sizeof(vcio.buf));
	return devopen(c, omode, vciodir, nelem(vciodir), devgen);
}

static void
vcioclose(Chan *c)
{
	free(c->aux);
	c->aux = nil;
}

static long
vcioread(Chan *c, void *a, long n, vlong off)
{
	long nr;

	lock(&vcio);
	if (waserror()) {
		unlock(&vcio);
		nexterror();
	}

	nr = 0;
	switch ((int)c->qid.path) {
	case Qdir:
		nr = devdirread(c, a, n, vciodir, nelem(vciodir), devgen);
		break;

	case Qvcio:
		if (off < vcio.len) {
			nr = MIN(n, vcio.len - off);
			memmove(a, vcio.buf + off, nr);
		}
		break;

	default:
		error(Egreg);
	}

	unlock(&vcio);
	poperror();

	return nr;
}

static long
vciowrite(Chan *c, void *a, long n, vlong)
{
	if (n & 3)
		error(Ebadarg);
	if (n < 4)
		error(Etoosmall);
	if (n > sizeof(vcio.buf))
		error(Etoobig);

	lock(&vcio);
	if (waserror()) {
		unlock(&vcio);
		nexterror();
	}

	switch ((int)c->qid.path) {
	case Qdir:
		error(Eperm);

	case Qvcio:
		memmove(c->aux, (char *)a + 4, n - 4);
		n = vcreq(leget4(a), c->aux, n - 4, sizeof(vcio.buf));
		if (n < 0) {
			memset(vcio.buf, 0, sizeof(vcio.buf));
			n = 0;
		} else
			memmove(vcio.buf, c->aux, sizeof(vcio.buf));
		vcio.len = n;
		break;

	default:
		error(Egreg);
	}

	unlock(&vcio);
	poperror();
	return n;
}

static void
vcioreset(void)
{
	memset(&vcio, 0, sizeof(vcio));
}

Dev vciodevtab = {
    'V',
    "vcio",

    vcioreset,
    devinit,
    devshutdown,
    vcioattach,
    vciowalk,
    vciostat,
    vcioopen,
    devcreate,
    vcioclose,
    vcioread,
    devbread,
    vciowrite,
    devbwrite,
    devremove,
    devwstat,
    devpower,
};
