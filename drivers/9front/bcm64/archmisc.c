#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"

static uint
getmodeltype(uint rev)
{
	static unsigned char map[] = {
	    1,       // B rev 1.0  2
	    1,       // B rev 1.0  3
	    1,       // B rev 2.0  4
	    1,       // B rev 2.0  5
	    1,       // B rev 2.0  6
	    0,       // A rev 2    7
	    0,       // A rev 2    8
	    0,       // A rev 2    9
	    0, 0, 0, // unused  a,b,c
	    1,       // B  rev 2.0  d
	    1,       // B rev 2.0  e
	    1,       // B rev 2.0  f
	    3,       // B+ rev 1.2 10
	    6,       // CM1        11
	    2,       // A+ rev1.1  12
	    3,       // B+ rev 1.2 13
	    6,       // CM1        14
	    2        // A+         15
	};

	if (!rev)
		return 0;

	// new style rev, bit 23 guarantees it
	if (rev & 0x800000)
		return (rev & 0xff0) >> 4;

	rev &= 0xffffff;
	if (rev < 2 || rev > 21)
		return 0;

	return map[rev - 2];
}

static char *
getmodelstr(int mod)
{
	switch (mod) {
	case 0x0:
		return "A";
	case 0x1:
		return "B";
	case 0x2:
		return "A+";
	case 0x3:
		return "B+";
	case 0x4:
		return "2B";
	case 0x5:
		return "Alpha";
	case 0x6:
		return "CM1";
	case 0x8:
		return "3B";
	case 0x9:
		return "Zero";
	case 0xa:
		return "CM3";
	case 0xc:
		return "Zero W";
	case 0xd:
		return "3B+";
	case 0xe:
		return "3A+";
	case 0xf:
		return "Internal";
	case 0x10:
		return "CM3+";
	case 0x11:
		return "4B";
	}
	return "unknown";
}

static long
vcinfo(Chan *, void *a, long n, vlong off)
{
	static struct {
		int id;
		char *name;
	} clks[] = {
	    {ClkEmmc, "emmc"},
	    {ClkUart, "uart"},
	    {ClkArm, "arm"},
	    {ClkCore, "core"},
	    {ClkV3d, "v3d"},
	    {ClkH264, "h264"},
	    {ClkIsp, "isp"},
	    {ClkSdram, "sdram"},
	    {ClkPixel, "pixel"},
	    {ClkPwm, "pwm"},
	    {ClkEmmc2, "emmc2"},
	};

	Confmem mem;
	uint rev, mod;
	ulong clkrate;
	char str[1024];
	int i, l;

	rev = getboardrev();
	mod = getmodeltype(rev);
	mem.base = mem.limit = 0;
	getramsize(&mem);

	l = 0;
	l += snprint(str + l, sizeof(str) - l, "rev %x\n", rev);
	l += snprint(str + l, sizeof(str) - l, "fw %x\n", getfirmware());
	l += snprint(str + l, sizeof(str) - l, "model type %s (%x)\n", getmodelstr(mod), mod);
	l += snprint(str + l, sizeof(str) - l, "mac %s\n", getethermac());
	l += snprint(str + l, sizeof(str) - l, "cputemp %d\n", getcputemp());
	l += snprint(str + l, sizeof(str) - l, "ram %llx %llx\n", mem.base, mem.limit);
	l += snprint(str + l, sizeof(str) - l, "pagesz %llx\n", BY2PG);

	for (i = 0; i < nelem(clks); i++) {
		clkrate = getclkrate(clks[i].id);
		l += snprint(str + l, sizeof(str) - l, "clk %s - %lux (%lud)\n", clks[i].name, clkrate, clkrate);
	}

	return readstr(off, a, n, str);
}

static long
bscinfo(Chan *, void *a, long n, vlong off)
{
	static uintptr masteraddr[] = {
	    0x205000,
	    0x804000,
	    0x805000,
	};
	static char *masterreg[] = {
	    "C",
	    "S",
	    "DLEN",
	    "A",
	    "FIFO",
	    "DIV",
	    "DEL",
	    "CLKT",
	};

	static uintptr slaveaddr[] = {
	    0x214000,
	};
	static char *slavereg[] = {
	    "DR",
	    "RSR",
	    "SLV",
	    "CR",
	    "FR",
	    "IFLS",
	    "IMSC",
	    "RIS",
	    "MIS",
	    "ICR",
	    "DMACR",
	    "TDR",
	    "GPUSTAT",
	    "HCTRL",
	    "DEBUG1",
	    "DEBUG2",
	};

	char str[1024];
	u32int *r;
	int i, j, l;

	l = 0;
	for (i = 0; i < nelem(masteraddr); i++) {
		r = (u32int *)(VIRTIO + masteraddr[i]);
		l += snprint(str + l, sizeof(str) - l, "BSC Master %d\n", i);
		for (j = 0; j < nelem(masterreg); j++)
			l += snprint(str + l, sizeof(str) - l, "%s %x\n", masterreg[j], r[j]);
		l += snprint(str + l, sizeof(str) - l, "\n");
	}

	for (i = 0; i < nelem(slaveaddr); i++) {
		r = (u32int *)(VIRTIO + slaveaddr[i]);
		l += snprint(str + l, sizeof(str) - l, "BSC Slave %d\n", i);
		for (j = 0; j < nelem(slavereg); j++)
			l += snprint(str + l, sizeof(str) - l, "%s %x\n", slavereg[j], r[j]);
		l += snprint(str + l, sizeof(str) - l, "\n");
	}

	return readstr(off, a, n, str);
}

enum {
	TS_CTRL = 0x0,
	TS_STAT = 0x1,
};

#define TS_STAT_VALID (1 << 10)
#define TS_STAT_DATA(x) ((x)&0x3ff)

static long
tempinfo(Chan *, void *a, long n, vlong off)
{
	char str[64];
	s64int m, c, t, u, s;
	u32int *r;

	r = (u32int *)(VIRTIO + 0x212000);
	m = -538;
	c = 412000;

	s = r[TS_STAT];
	t = m * TS_STAT_DATA(s) + c;
	t = 273150000 + 1000 * t;
	u = 10000000;
	snprint(str, sizeof(str), "%lld.%lld %d\n",
	        t / u, t % u, !!(s & TS_STAT_VALID));

	return readstr(off, a, n, str);
}

static long
gpioinfo(Chan *, void *a, long n, vlong off)
{
	static char *fsel[] = {
	    "in",
	    "out",
	    "alt5",
	    "alt4",
	    "alt0",
	    "alt1",
	    "alt2",
	    "alt3",
	};

	char str[1536];
	int f, i, j, l, s, v;
	u32int *r;

	r = (u32int *)(VIRTIO + 0x200000);
	l = 0;
	l += snprint(str + l, sizeof(str) - l, "gpio\n");
	for (i = 0; i < 54; i++) {
		f = i / 10;
		s = (i % 10) * 3;
		j = (r[f] >> s) & 0x7;

		v = (r[0xd + i / 32] >> (i & 31)) & 1;

		l += snprint(str + l, sizeof(str) - l, "%d level=%d fsel=%d func=%s\n", i, v, j, fsel[j]);
	}

	l += snprint(str + l, sizeof(str) - l, "\nfsel\n");
	for (i = 0; i < nelem(fsel); i++)
		l += snprint(str + l, sizeof(str) - l, "%d %s\n", i, fsel[i]);

	return readstr(off, a, n, str);
}

static long
dmainfo(Chan *, void *a, long n, vlong off)
{
	char *str;
	u32int *r;
	int i, l, nr;

	str = smalloc(READSTR);
	r = (u32int *)(VIRTIO + 0x7000);
	l = 0;
	l += snprint(str + l, READSTR - l, "interrupt status %ux\n", r[0xfe0 >> 2]);
	l += snprint(str + l, READSTR - l, "enable           %ux\n", r[0xff0 >> 2]);
	l += snprint(str + l, READSTR - l, "\n");
	for (i = 0; i <= 15; i++) {
		r = (u32int *)(VIRTIO + 0x7000 + 0x100 * i);
		if (i == 15)
			r = (u32int *)(VIRTIO + 0xe05000);

		l += snprint(str + l, READSTR - l, "dma %d\n", i);
		l += snprint(str + l, READSTR - l, "control               %ux\n", r[0]);
		l += snprint(str + l, READSTR - l, "control block address %ux\n", r[1]);
		l += snprint(str + l, READSTR - l, "transfer information  %ux\n", r[2]);
		l += snprint(str + l, READSTR - l, "source address        %ux\n", r[3]);
		l += snprint(str + l, READSTR - l, "dest address          %ux\n", r[4]);
		l += snprint(str + l, READSTR - l, "transfer length       %ux\n", r[5]);
		l += snprint(str + l, READSTR - l, "stride                %ux\n", r[6]);
		l += snprint(str + l, READSTR - l, "next cb address       %ux\n", r[7]);
		l += snprint(str + l, READSTR - l, "debug                 %ux\n", r[8]);
		l += snprint(str + l, READSTR - l, "\n");
	}
	nr = readstr(off, a, n, str);
	free(str);
	return nr;
}

static void
addsegmaps(void)
{
	static struct {
		char *name;
		uintptr physoff;
		ulong size;
		int attr;
	} tab[] = {
	    {"virtio", 0, 16 * MiB, SG_PHYSICAL | SG_DEVICE | SG_NOEXEC},
	};

	Physseg seg;
	int i;

	memset(&seg, 0, sizeof(seg));
	for (i = 0; i < nelem(tab); i++) {
		seg.name = tab[i].name;
		seg.pa = tab[i].physoff + soc.physio;
		seg.size = tab[i].size;
		seg.attr = tab[i].attr;
		addphysseg(&seg);
	}
}

void
archmisclink(void)
{
	addarchfile("vcinfo", 0444, vcinfo, nil);
	addarchfile("bscinfo", 0444, bscinfo, nil);
	addarchfile("tempinfo", 0444, tempinfo, nil);
	addarchfile("gpioinfo", 0444, gpioinfo, nil);
	addarchfile("dmainfo", 0444, dmainfo, nil);
	addsegmaps();
}
