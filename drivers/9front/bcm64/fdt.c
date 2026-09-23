#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"

static uchar *fdt;
static ulong fdtlen;

static u32int
beget4(uchar *p)
{
	return (u32int)p[0] << 24 | (u32int)p[1] << 16 | (u32int)p[2] << 8 | (u32int)p[3];
}

int
fdtinit(uintptr pa)
{
	uchar *a, *b, *p;
	u32int mask, off, len;

	a = b = nil;
	len = 0;
	if (pa == 0 || cankaddr(pa) == 0)
		goto out;

	mask = ~(BY2PG - 1);
	off = pa & (BY2PG - 1);
	a = vmap(pa & mask, 2 * BY2PG);
	if (a == nil)
		goto out;

	p = a + off;
	if (beget4(p) != 0xd00dfeed)
		goto out;

	len = beget4(p + 4);
	if (len < 28)
		goto out;

	b = vmap(pa & mask, PGROUND(len));
	if (b == nil)
		goto out;

	fdt = mallocalign(PGROUND(len), BY2PG, 0, 0);
	if (fdt != nil) {
		memmove(fdt, b, PGROUND(len));
		fdt += off;
		fdtlen = len;
	}

out:
	if (a != nil)
		vunmap(a, 2 * BY2PG);

	if (b != nil)
		vunmap(b, PGROUND(len));

	return (fdt != nil) ? 0 : -1;
}

static long
fdtread(Chan *, void *a, long n, vlong off)
{
	if (fdt == nil || off >= fdtlen)
		return 0;

	n = MIN(fdtlen - off, n);
	memmove(a, fdt + off, n);
	return n;
}

void
fdtlink(void)
{
	addarchfile("fdt", 0444, fdtread, nil);
}
