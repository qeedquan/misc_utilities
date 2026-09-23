#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "../port/error.h"
#include "io.h"

#define divroundup(n, d) (((n) + (d)-1) / (d))
#define max(a, b) (((a) > (b)) ? (a) : (b))

enum {
	BSC_C = 0x0,
	BSC_S,
	BSC_DLEN,
	BSC_A,
	BSC_FIFO,
	BSC_DIV,
	BSC_DEL,
	BSC_CLKT,
};

enum {
	BSC_C_READ = 1 << 0,
	BSC_C_CLEAR = 1 << 4,
	BSC_C_ST = 1 << 7,
	BSC_C_INTD = 1 << 8,
	BSC_C_INTT = 1 << 9,
	BSC_C_INTR = 1 << 10,
	BSC_C_I2CEN = 1 << 15,
};

enum {
	BSC_S_TA = 1 << 0,
	BSC_S_DONE = 1 << 1,
	BSC_S_TXW = 1 << 2,
	BSC_S_RXR = 1 << 3,
	BSC_S_TXD = 1 << 4,
	BSC_S_RXD = 1 << 5,
	BSC_S_TXE = 1 << 6,
	BSC_S_RXF = 1 << 7,
	BSC_S_ERR = 1 << 8,
	BSC_S_CLKT = 1 << 9,

	BSC_S_LEN = 1 << 10,
};

enum {
	Qctl = 1,
	Qdata,
};

typedef struct I2C I2C;

struct I2C {
	QLock;
	Rendez;
	Lock intr;

	u32int *regs;
	int timeout;
	char errmsg[64];

	u8int md[256];
	u8int *mb;
	long mblen;
	int mberr;
	int mbdone;
};

static I2C i2c[1];
static Dirtab i2cdir[2 * nelem(i2c) + 1];

#define TYPE(x) ((x)&3)
#define ID(x) ((x) >> 2)
#define QID(i, t) (((i) << 2) | (t))

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

static int
setupclk(I2C *ic, ulong rate)
{
	ulong prate, div, redl, fedl, clktout;

	prate = getclkrate(ClkV3d);
	div = divroundup(prate, rate);
	if (div & 1)
		div++;
	if (div < 0x2 || div > 0xfffe)
		return -1;

	ic->regs[BSC_DIV] = div;

	fedl = max(div / 16, 1);
	redl = max(div / 4, 1);
	ic->regs[BSC_DEL] = fedl << 16 | redl;

	if (rate > 0xffff * 1000 / 35)
		clktout = 0xffff;
	else
		clktout = 35 * rate / 1000;
	ic->regs[BSC_CLKT] = clktout;

	return 0;
}

static Chan *
i2cattach(char *spec)
{
	return devattach('2', spec);
}

static Walkqid *
i2cwalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, i2cdir, nelem(i2cdir), devgen);
}

static int
i2cstat(Chan *c, uchar *dp, int n)
{
	return devstat(c, dp, n, i2cdir, nelem(i2cdir), devgen);
}

static Chan *
i2copen(Chan *c, int omode)
{
	return devopen(c, omode, i2cdir, nelem(i2cdir), devgen);
}

static void
i2cclose(Chan *)
{
}

static void
drainrx(I2C *ic)
{
	u32int v;

	while (ic->mblen) {
		v = ic->regs[BSC_S];
		if (!(v & BSC_S_RXD))
			break;
		*ic->mb = ic->regs[BSC_FIFO];
		ic->mb++;
		ic->mblen--;
	}
}

static void
filltx(I2C *ic)
{
	u32int v;

	while (ic->mblen) {
		v = ic->regs[BSC_S];
		if (!(v & BSC_S_TXD))
			break;
		ic->regs[BSC_FIFO] = *ic->mb;
		ic->mb++;
		ic->mblen--;
	}
}

static void
inthandle(I2C *ic)
{
	u32int val, err;

	val = ic->regs[BSC_S];
	err = val & (BSC_S_CLKT | BSC_S_ERR);
	if (err) {
		ic->mberr = err;
		goto complete;
	}

	if (val & BSC_S_DONE) {
		drainrx(ic);

		if (ic->mblen != 0)
			ic->mberr = BSC_S_LEN;

		goto complete;
	}

	if (val & BSC_S_TXW) {
		if (ic->mblen == 0) {
			ic->mberr = val | BSC_S_LEN;
			goto complete;
		}

		filltx(ic);
		return;
	}

	if (val & BSC_S_RXR) {
		if (ic->mblen == 0) {
			ic->mberr = val | BSC_S_LEN;
			goto complete;
		}
		drainrx(ic);
		return;
	}

complete:
	ic->regs[BSC_C] = BSC_C_CLEAR;
	ic->regs[BSC_S] = BSC_S_CLKT | BSC_S_ERR | BSC_S_DONE;
	ic->mbdone = 1;
	wakeup(ic);
}

static void
i2cint(Ureg *, void *)
{
	I2C *ic;
	int i;

	coherence();
	for (i = 0; i < nelem(i2c); i++) {
		ic = i2c + i;
		ilock(&ic->intr);
		inthandle(ic);
		iunlock(&ic->intr);
	}
	coherence();
}

static long
xfer(I2C *ic, int op, void *a, long n)
{
	u32int c;

	if (n == 0)
		return 0;

	if (n >= nelem(ic->md))
		error(Etoobig);

	c = BSC_C_ST | BSC_C_I2CEN;
	if (op == 'r')
		c |= BSC_C_READ | BSC_C_INTR;
	else {
		memmove(ic->md, a, n);
		c |= BSC_C_INTT;
	}

	c |= BSC_C_INTD;

	ilock(&ic->intr);
	ic->mb = ic->md;
	ic->mblen = n;
	ic->mberr = 0;
	ic->mbdone = 0;
	ic->regs[BSC_DLEN] = ic->mblen;
	ic->regs[BSC_C] = c;
	iunlock(&ic->intr);

	if (waserror()) {
		ic->regs[BSC_C] = 0;
		nexterror();
	}

	tsleep(ic, return0, nil, ic->timeout);
	ilock(&ic->intr);
	if (ic->mberr) {
		snprint(ic->errmsg, sizeof(ic->errmsg), "i2c: transfer failed: %x\n", ic->mberr);
		n = -1;
	} else if (!ic->mbdone) {
		snprint(ic->errmsg, sizeof(ic->errmsg), "i2c: transfer timed out\n");
		n = -1;
	}
	iunlock(&ic->intr);

	poperror();
	return n;
}

static long
i2cread(Chan *c, void *a, long n, vlong off)
{
	I2C *ic;
	char str[128];
	long nr;
	int l;
	u64int div, rate;

	if (c->qid.type & QTDIR)
		return devdirread(c, a, n, i2cdir, nelem(i2cdir), devgen);

	ic = i2c + ID(c->qid.path);

	qlock(ic);
	if (waserror()) {
		qunlock(ic);
		nexterror();
	}

	nr = 0;
	switch (TYPE(c->qid.path)) {
	case Qctl:
		div = ic->regs[BSC_DIV];
		rate = getclkrate(ClkV3d) / div;

		l = 0;
		l += snprint(str + l, sizeof(str) - l, "rate %lld hz (div %llx del %ux)\n", rate, div, ic->regs[BSC_DEL]);
		l += snprint(str + l, sizeof(str) - l, "timeout %d ms\n", ic->timeout);
		l += snprint(str + l, sizeof(str) - l, "clock stretch %ux cycles\n", ic->regs[BSC_CLKT]);
		snprint(str + l, sizeof(str) - l, "slave addr %x\n", ic->regs[BSC_A]);
		nr = readstr(off, a, n, str);
		break;

	case Qdata:
		if (xfer(ic, 'r', a, n) < 0)
			error(ic->errmsg);
		memmove(a, ic->md, n);
		nr = n;
		break;
	}

	qunlock(ic);
	poperror();

	return nr;
}

static long
i2cwrite(Chan *c, void *a, long n, vlong)
{
	I2C *ic;
	char str[128], *tok[2];
	int timeout, addr;

	if (c->qid.type & QTDIR)
		error(Eperm);

	ic = i2c + ID(c->qid.path);
	qlock(ic);
	if (waserror()) {
		qunlock(ic);
		nexterror();
	}

	switch (TYPE(c->qid.path)) {
	case Qctl:
		if (n >= sizeof(str))
			error(Etoobig);

		memmove(str, a, n);
		str[n] = '\0';
		tokenize(str, tok, nelem(tok));
		if (tok[0] == nil || tok[1] == nil)
			break;

		if (!strcmp(tok[0], "rate")) {
			if (setupclk(ic, xatoul(tok[1])) < 0)
				error(Ebadarg);
		} else if (!strcmp(tok[0], "timeout")) {
			timeout = xatoul(tok[1]);
			if (timeout <= 0)
				error(Ebadarg);

			ic->timeout = timeout;
		} else if (!strcmp(tok[0], "slave")) {
			addr = xatoul(tok[1]);
			if (!(0 <= addr && addr <= 0x7f))
				error(Ebadarg);

			ic->regs[BSC_A] = addr;
		} else if (!strcmp(tok[0], "enable")) {
			gpiosel(2, 4);
			gpiosel(3, 4);
		} else
			error(Ebadarg);

		break;

	case Qdata:
		if (xfer(ic, 'w', a, n) < 0)
			error(ic->errmsg);
		break;
	}

	qunlock(ic);
	poperror();

	return n;
}

static void
i2cinit(I2C *ic)
{
	setupclk(ic, 100000);
	ic->regs[BSC_C] = 0;
	ic->timeout = 1000;
}

static void
i2cprobe(void)
{
	Dirtab *dp;
	ulong i;

	memset(i2c, 0, sizeof(i2c));
	i2c[0].regs = (u32int *)(VIRTIO + 0x804000);
	for (i = 0; i < nelem(i2c); i++)
		i2cinit(i2c + i);

	dp = i2cdir;
	strcpy(dp->name, ".");
	mkqid(&dp->qid, 0, 0, QTDIR);
	dp->length = 0;
	dp->perm = DMDIR | 0555;
	dp++;
	for (i = 0; i < nelem(i2c); i++) {
		snprint(dp->name, sizeof(dp->name), "i2c%ludctl", i + 1);
		dp->qid.path = QID(i, Qctl);
		dp->perm = 0666;
		dp++;

		snprint(dp->name, sizeof(dp->name), "i2c%luddata", i + 1);
		dp->qid.path = QID(i, Qdata);
		dp->perm = 0666;
		dp++;
	}

	intrenable(IRQi2c, i2cint, nil, BUSUNKNOWN, "i2c");
}

static void
i2creset(void)
{
	i2cprobe();
}

Dev i2cdevtab = {
    '2',
    "i2c",

    i2creset,
    devinit,
    devshutdown,
    i2cattach,
    i2cwalk,
    i2cstat,
    i2copen,
    devcreate,
    i2cclose,
    i2cread,
    devbread,
    i2cwrite,
    devbwrite,
    devremove,
    devwstat,
    devpower,
};
