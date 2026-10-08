// toggle some pins at a fixed rate, usually for blinking leds

#include <u.h>
#include <libc.h>
#include <bio.h>

typedef struct {
	u32int *mmio;
} GPIO;

int
getpagesize(void)
{
	int pgsize, n;
	char *s, *f[2];
	Biobuf *bp;

	pgsize = 0x1000;
	bp = Bopen("/dev/swap", OREAD);
	if (bp == nil)
		return pgsize;

	while ((s = Brdline(bp, '\n'))) {
		if ((n = Blinelen(bp)) < 1)
			continue;
		s[n - 1] = '\0';
		if (tokenize(s, f, nelem(f)) != 2)
			continue;
		if (strcmp(f[1], "pagesize") == 0) {
			pgsize = strtoul(f[0], 0, 0);
			break;
		}
	}
	Bterm(bp);

	return pgsize;
}

int
gpioopen(GPIO *g)
{
	g->mmio = segattach(0, "gpio", 0, getpagesize());
	if (g->mmio == (void *)-1)
		return -1;
	return 0;
}

void
gpioclose(GPIO *g)
{
	segdetach(g->mmio);
}

void
gpioin(GPIO *g, u32int p)
{
	g->mmio[p / 10] &= ~(7 << (p % 10) * 3);
}

void
gpioout(GPIO *g, u32int p)
{
	g->mmio[p / 10] |= (1 << ((p % 10) * 3));
}

void
gpioset(GPIO *g, u32int v)
{
	g->mmio[7] = v;
}

void
gpioclr(GPIO *g, u32int v)
{
	g->mmio[10] = v;
}

void
usage(void)
{
	fprint(2, "usage: [options] pin\n");
	fprint(2, "  -b:    blink time (ms)\n");
	fprint(2, "  -h:    show this message\n");
	exits("usage");
}

int interrupted = 0;

void
interrupt(void *a, char *s)
{
	interrupted = 1;
	noted(NCONT);
	USED(a);
	USED(s);
}

void
main(int argc, char *argv[])
{
	int pin, blink;
	GPIO gpio;

	blink = 500;
	ARGBEGIN
	{
	case 'b':
		blink = atoi(EARGF(usage()));
		break;
	case 'h':
	default:
		usage();
	}
	ARGEND;

	if (argc < 1)
		usage();
	pin = atoi(argv[0]);

	if (gpioopen(&gpio) < 0)
		sysfatal("blink: failed to open gpio device: %r\n");

	notify(interrupt);

	print("blinking pin %d at %d ms\n", pin, blink);
	gpioin(&gpio, pin);
	gpioout(&gpio, pin);
	for (;;) {
		gpioset(&gpio, 1 << pin);
		sleep(blink);
		gpioclr(&gpio, 1 << pin);
		sleep(blink);
		if (interrupted)
			break;
	}

	gpioclose(&gpio);

	exits(nil);
}
