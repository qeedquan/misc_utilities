#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"

#define RNGREGS (VIRTIO + 0x104000)

enum {
	RNG_CTRL = 0x0,
	RNG_STAT,
	RNG_DATA
};

#define RNG_CTRL_EN (1 << 0)
#define RNG_STAT_COUNT(x) ((x) >> 24)

static int sethwrand = 0;
static QLock rnglock;

static u32int
waitc(void)
{
	u32int *r, c;

	r = (u32int *)RNGREGS;
	for (;;) {
		c = RNG_STAT_COUNT(r[RNG_STAT]);
		if (c > 0)
			return c;
		tsleep(&up->sleep, return0, nil, 10);
	}
}

static void
fill(void *a, ulong n)
{
	u32int *r, c, v;
	char *p;

	qlock(&rnglock);
	if (waserror()) {
		qunlock(&rnglock);
		nexterror();
	}

	r = (u32int *)RNGREGS;
	p = a;
	c = waitc();
	if (n & 3) {
		v = r[RNG_DATA];
		memmove(p, &v, n & 3);
		p += (n & 3);
		n &= ~3;
		c--;
	}

	while (n) {
		for (; c && n; c--) {
			v = r[RNG_DATA];
			memmove(p, &v, 4);
			p += 4;
			n -= 4;
		}
		c = waitc();
	}

	qunlock(&rnglock);
	poperror();
}

static long
rngread(Chan *, void *a, long n, vlong)
{
	fill(a, n);
	return n;
}

static void
rngprobe(void)
{
	u32int *r;

	r = (u32int *)RNGREGS;
	r[RNG_STAT] = 250000;
	r[RNG_CTRL] = RNG_CTRL_EN;
}

void
rnglink(void)
{
	rngprobe();
	addarchfile("rng", 0444, rngread, nil);
	if (sethwrand)
		hwrandbuf = fill;
}
