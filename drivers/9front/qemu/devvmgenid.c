/* reads qemu vmgenid device guid */
#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../port/error.h"
#include "io.h"
#include <aml.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))

typedef struct Header Header;
typedef struct Vmid Vmid;

struct Header {
	uchar sig[4];
	uchar len[4];
	uchar rev;
	uchar csum;
	uchar oemid[6];
	uchar oemtid[8];
	uchar oemrev[4];
	uchar cid[4];
	uchar crev[4];
};

enum {
	Hdrsz = 4 + 4 + 1 + 1 + 6 + 8 + 4 + 4 + 4,
};

struct Vmid {
	uchar *addr;
};

enum {
	Vmgeniddirqid,
	Vmgenidqid,
};

Dirtab vmgenidtab[] = {
    {".", {Vmgeniddirqid, 0, QTDIR}, 0, DMDIR | 0550},
    {"vmgenid", {Vmgenidqid}, 0, 0666},
};

static Vmid vmid;

static ulong
get32(uchar *p)
{
	return p[3] << 24 | p[2] << 16 | p[1] << 8 | p[0];
}

static long
readn(Chan *c, void *vp, long n)
{
	char *p = vp;
	long nn, nr;

	nr = 0;
	while (n > 0) {
		nn = devtab[c->type]->read(c, p, n, c->offset);
		if (nn == 0)
			break;
		c->offset += nn;
		p += nn;
		n -= nn;
		nr += nn;
	}
	return nr;
}

static long
discard(Chan *c, long n)
{
	char buf[128];
	long nn, nr, mn;

	nr = 0;
	for (;;) {
		mn = min(sizeof(buf), n);
		nn = readn(c, buf, mn);
		if (nn == 0 || nn != mn)
			break;
		n -= nn;
		nr += nn;
	}
	return nr;
}

static int
getaddr(void *dot, void *)
{
	void *r;

	if (amleval(dot, "", &r) == 0)
		vmid.addr = vmap(amlint(r), 0x1000);
	return 1;
}

static void
vmgenidprobe(void)
{
	Chan *c;
	Header h;
	uchar *data;
	ulong l, n;

	if (!iseve())
		error(Eperm);

	c = namec("/dev/acpitbls", Aopen, OREAD, 0);

	if (waserror()) {
		amlexit();
		free(data);
		cclose(c);
		nexterror();
	}

	amlinit();
	for (;;) {
		if (readn(c, &h, Hdrsz) != Hdrsz)
			break;

		l = get32(h.len);
		if (l <= Hdrsz)
			break;
		l -= Hdrsz;

		if (memcmp(h.sig, "SSDT", 4) != 0) {
			if (discard(c, l) != l)
				break;
			continue;
		}

		data = malloc(l);
		if (data == nil)
			break;

		n = readn(c, data, l);
		if (n == l) {
			amlload(data, l);
			amlenum(amlroot, "VGIA", getaddr, nil);
		}
		break;
	}

	poperror();
	amlexit();
	free(data);
	cclose(c);
}

static Chan *
vmgenidattach(char *spec)
{
	vmgenidprobe();
	return devattach('V', spec);
}

static Walkqid *
vmgenidwalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, vmgenidtab, nelem(vmgenidtab), devgen);
}

static int
vmgenidstat(Chan *c, uchar *dp, int n)
{
	return devstat(c, dp, n, vmgenidtab, nelem(vmgenidtab), devgen);
}

static Chan *
vmgenidopen(Chan *c, int omode)
{
	if (c->qid.type == QTDIR) {
		if (omode != OREAD)
			error(Eperm);
	}
	c->mode = openmode(omode);
	c->flag |= COPEN;
	c->offset = 0;
	return c;
}

static void
vmgenidclose(Chan *)
{
}

static long
vmgenidread(Chan *c, void *a, long n, vlong off)
{
	char buf[64];
	uchar guid[16];
	int nr;

	nr = 0;
	switch ((int)c->qid.path) {
	case Vmgeniddirqid:
		nr = devdirread(c, a, n, vmgenidtab, nelem(vmgenidtab), devgen);
		break;

	case Vmgenidqid:
		memset(guid, 0, sizeof(guid));
		if (vmid.addr != nil)
			memmove(guid, &vmid.addr[40], sizeof(guid));
		snprint(buf, sizeof(buf), "%x%x%x%x-%x%x-%x%x-%x%x-%x%x%x%x%x%x\n",
		        guid[3], guid[2], guid[1], guid[0], guid[5], guid[4], guid[7], guid[6],
		        guid[8], guid[9], guid[10], guid[11], guid[12], guid[13], guid[14], guid[15]);
		nr = readstr(off, a, n, buf);
		break;
	}
	return nr;
}

static long
vmgenidwrite(Chan *, void *, long, vlong)
{
	error(Ebadusefd);
	return 0;
}

Dev vmgeniddevtab = {
    'V',
    "vmgenid",

    devreset,
    devinit,
    devshutdown,
    vmgenidattach,
    vmgenidwalk,
    vmgenidstat,
    vmgenidopen,
    devcreate,
    vmgenidclose,
    vmgenidread,
    devbread,
    vmgenidwrite,
    devbwrite,
    devremove,
    devwstat,
    devpower,
};
