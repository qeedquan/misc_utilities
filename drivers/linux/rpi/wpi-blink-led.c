#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <getopt.h>
#include <err.h>
#include <wiringPi.h>

static volatile sig_atomic_t interrupted;

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
	fprintf(stderr, " -b:  blink time (ms)\n");
	fprintf(stderr, " -h:  show this message\n");
	fprintf(stderr, " -v:  be more verbose\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	long blink;
	int led, verbose, c;

	signal(SIGINT, interrupt);
	blink = 1000;
	verbose = 0;
	while ((c = getopt(argc, argv, "b:hv")) != -1) {
		switch (c) {
		case 'b':
			blink = atol(optarg);
			break;
		case 'v':
			verbose = 1;
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

	if (verbose)
		setenv("WIRINGPI_DEBUG", "1", 1);
	if (wiringPiSetup() < 0)
		errx(1, "failed to setup wiring pi");

	pinMode(led, OUTPUT);
	printf("Blinking LED %d (BCM_PIN %d) at %ld ms\n", led, wpiPinToGpio(led), blink);
	for (;;) {
		digitalWrite(led, HIGH);
		delay(blink);
		digitalWrite(led, LOW);
		delay(blink);
		if (interrupted)
			break;
	}
	return 0;
}
