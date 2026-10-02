#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <pigpio.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

typedef struct {
	int pin[64];
	size_t npin;
	long servo_pulsewidth;
	struct timespec toggle_delay;
	struct timespec write_delay;
} Option;

void
usage(void)
{
	fprintf(stderr, "usage: [option] pin\n");
	fprintf(stderr, "\t-r <us>: use servo with pulse width\n");
	fprintf(stderr, "\t-t <us>: delay between toggles\n");
	fprintf(stderr, "\t-w <us>: delay until next write\n");
	exit(2);
}

struct timespec
us2ts(long long us)
{
	return (struct timespec){
	    .tv_sec = us / 1000000ull,
	    .tv_nsec = (us % 1000000ull) * 1000,
	};
}

void
parseopt(Option *o, int argc, char *argv[])
{
	long long toggle_delay, write_delay;
	int c, i;

	memset(o, 0, sizeof(*o));
	toggle_delay = write_delay = 0;
	while ((c = getopt(argc, argv, "hr:t:w:")) != -1) {
		switch (c) {
		case 'r':
			o->servo_pulsewidth = atol(optarg);
			break;
		case 't':
			sscanf(optarg, "%lld", &toggle_delay);
			break;
		case 'w':
			sscanf(optarg, "%lld", &write_delay);
			break;
		case 'h':
			usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc == 0)
		usage();

	o->write_delay = us2ts(write_delay);
	o->toggle_delay = us2ts(toggle_delay);
	for (i = 0; i < argc; i++) {
		if (o->npin >= nelem(o->pin))
			break;

		o->pin[o->npin++] = atoi(argv[i]);
	}
}

void
delay(struct timespec *tp)
{
	if (tp->tv_sec == 0 && tp->tv_nsec == 0)
		return;

	clock_nanosleep(CLOCK_MONOTONIC, 0, tp, NULL);
}

int
main(int argc, char *argv[])
{
	Option opt;
	size_t i;

	parseopt(&opt, argc, argv);

	if (gpioInitialise() < 0)
		exit(1);

	printf("Hardware revision: %x\n", gpioHardwareRevision());
	printf("Toggle Delay %ld seconds %ld nanoseconds\n", (long)opt.toggle_delay.tv_sec, opt.toggle_delay.tv_nsec);
	printf("Write Delay %ld seconds %ld nanoseconds\n", (long)opt.write_delay.tv_sec, opt.write_delay.tv_nsec);
	if (opt.servo_pulsewidth)
		printf("Servo Pulse Width %ld\n", opt.servo_pulsewidth);

	for (i = 0; i < opt.npin; i++) {
		gpioSetMode(opt.pin[i], PI_OUTPUT);
		if (opt.servo_pulsewidth)
			gpioServo(opt.pin[i], opt.servo_pulsewidth);
	}

	for (;;) {
		if (opt.servo_pulsewidth) {
			delay(&(struct timespec){1, 0});
			continue;
		}

		for (i = 0; i < opt.npin; i++) {
			gpioWrite(opt.pin[i], 1);
			delay(&opt.toggle_delay);
			gpioWrite(opt.pin[i], 0);
		}
		delay(&opt.write_delay);
	}

	return 0;
}
