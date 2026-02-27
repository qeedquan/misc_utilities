// https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
// https://pieter-jan.com/node/15

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <err.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>

typedef struct {
	int revision;
} Option;

typedef struct {
	int fd;
	volatile uint32_t *addr;
} GPIO;

Option opt = {
    .revision = 2,
};

GPIO gpio;

volatile sig_atomic_t interrupted;

void
interrupt(int sig)
{
	interrupted = 1;
	(void)sig;
}

void
usage(void)
{
	fprintf(stderr, "usage: [options] led\n");
	fprintf(stderr, "  -b: blink speed (ms)\n");
	fprintf(stderr, "  -h: show this message\n");
	fprintf(stderr, "  -r: specify rpi board revision (default: %d)\n", opt.revision);
	exit(2);
}

void
millisleep(long ms)
{
	struct timespec tp = {
	    .tv_sec = ms / 1000,
	    .tv_nsec = (ms % 1000) * 1000000,
	};
	nanosleep(&tp, NULL);
}

uintptr_t
gpiobase(int revision)
{
	switch (revision) {
	case 1:
		return 0x20000000;
	default:
		return 0x3f000000;
	}
}

int
gpioopen(GPIO *g, uintptr_t base)
{
	int fd, flags;
	void *addr;
	size_t pagesize;

	pagesize = getpagesize();
	flags = O_RDWR | O_SYNC | O_CLOEXEC;
	fd = open("/dev/mem", flags);
	if (fd < 0)
		fd = open("/dev/gpiomem", flags);
	if (fd < 0)
		goto err;

	addr = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base + 0x200000);
	if (addr == MAP_FAILED)
		goto err;

	g->fd = fd;
	g->addr = addr;

	return 0;

err:
	if (addr)
		munmap(addr, pagesize);
	if (fd >= 0)
		close(fd);
	return -errno;
}

void
gpioclose(GPIO *g)
{
	munmap((void *)g->addr, getpagesize());
	close(g->fd);
}

void
gpioin(GPIO *g, uint32_t p)
{
	g->addr[p / 10] &= ~(7 << (p % 10) * 3);
}

void
gpioout(GPIO *g, uint32_t p)
{
	g->addr[p / 10] |= (1 << ((p % 10) * 3));
}

void
gpioset(GPIO *g, uint32_t v)
{
	g->addr[7] = v;
}

void
gpioclr(GPIO *g, uint32_t v)
{
	g->addr[10] = v;
}

int
main(int argc, char *argv[])
{
	long blink;
	int led, c;

	signal(SIGINT, interrupt);

	blink = 500;
	while ((c = getopt(argc, argv, "b:hr:")) != -1) {
		switch (c) {
		case 'b':
			blink = atol(optarg);
			break;
		case 'r':
			opt.revision = atoi(optarg);
			break;
		case 'h':
		default:
			usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;
	if (argc == 0)
		usage();
	led = atoi(argv[0]);

	if (gpioopen(&gpio, gpiobase(opt.revision)) < 0)
		err(1, "opengpio");

	gpioin(&gpio, led);
	gpioout(&gpio, led);

	printf("Blinking LED %d (BCM_PIN %d) at %ld ms\n", led, led, blink);
	for (;;) {
		gpioset(&gpio, 1 << led);
		millisleep(blink);
		gpioclr(&gpio, 1 << led);
		millisleep(blink);
		if (interrupted)
			break;
	}

	gpioclose(&gpio);

	return 0;
}
