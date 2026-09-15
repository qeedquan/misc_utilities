#include "u.h"
#include "../port/lib.h"
#include "mem.h"
#include "dat.h"
#include "fns.h"
#include "io.h"

static char *irqnames[] = {
    [IRQtimer0] "timer0",
    [IRQtimer1] "timer1",
    [IRQtimer2] "timer2",
    [IRQtimer3] "timer3",
    [IRQusb] "usb",
    [IRQdma0] "dma0",
    [IRQaux] "aux",
    [IRQi2c] "i2c",
    [IRQspi] "spi",
    [IRQsdhost] "sdhost",
    [IRQuart] "uart",
    [IRQmmc] "mmc",
    [IRQtimerArm] "armtimer",
    [IRQcntps] "cntps",
    [IRQmbox0] "mbox0",
    [IRQmbox1] "mbox1",
    [IRQmbox2] "mbox2",
    [IRQmbox3] "mbox3",
    [IRQlocaltmr] "localtimer",
    [IRQgic] "gic",
    [IRQpci] "pci",
    [IRQether] "ether",
};

static uvlong irqcnts[MAXMACH][nelem(irqnames) + 1];

static long
irqstatsread(Chan *, void *a, long n, vlong off)
{
	Mach *mp;
	char *str;
	int i, j;
	long l, nr;

	str = smalloc(READSTR);
	l = 0;
	for (i = 0; i < MAXMACH; i++) {
		if (!active.machs[i])
			continue;

		mp = MACHP(i);
		l += snprint(str + l, READSTR - l, "cpu %d total %d\n", i, mp->intr);
		for (j = 0; j < nelem(irqnames); j++) {
			if (irqnames[j] == nil)
				continue;

			l += snprint(str + l, READSTR - l, "%3d %-12s %llud\n", j, irqnames[j], irqcnts[i][j]);
		}
		l += snprint(str + l, READSTR - l, "%3d %-12s %llud\n", j, "unknown", irqcnts[i][j]);

		if (i + 1 < MAXMACH)
			l += snprint(str + l, READSTR - l, "\n");
	}

	nr = readstr(off, a, n, str);
	free(str);
	return nr;
}

void
irqstatsinc(int machno, int irq)
{
	if (!(0 <= machno && machno < MAXMACH))
		return;

	if (!(0 <= irq && irq < nelem(irqnames)) || irqnames[irq] == nil)
		irq = nelem(irqcnts) - 1;

	irqcnts[machno][irq]++;
}

void
irqstatslink(void)
{
	addarchfile("irqstats", 0444, irqstatsread, nil);
}
