// based on https://github.com/PeterLemon/RaspberryPi

#include "libc.h"
#include "vio.h"
#include "util.h"

volatile sig_atomic_t done;

vio_t vi[1];

void
sigint(int sig)
{
	done = 1;
	(void)sig;
}

void
usage(void)
{
	fprintf(stderr, "usage: [options]\n");
	fprintf(stderr, "  -d <div>      specify divider\n");
	fprintf(stderr, "  -c <source>   specify clock source\n");
	fprintf(stderr, "  -h            show this message\n");
	fprintf(stderr, "  -r <range>    specify range\n");
	fprintf(stderr, "usage: [options]\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	static const int clksrc[] = {
	    CM_SRC_OSC,
	    CM_SRC_PLLCPER,
	    CM_SRC_PLLDPER,
	};

	time_t start;
	unsigned long count, val;
	uint32_t range, idiv, idivf;
	int c, clk, rv;
	double div, divi, divf;

	clk = CM_SRC_PLLCPER;
	range = 0xaa1;
	div = 2;
	while ((c = getopt(argc, argv, "c:d:hr:")) != -1) {
		switch (c) {
		case 'c':
			val = strtoul(optarg, NULL, 0);
			if (val < nelem(clksrc))
				clk = clksrc[val];
			break;
		case 'd':
			div = strtod(optarg, NULL);
			break;
		case 'r':
			range = strtoul(optarg, NULL, 0);
			break;
		case 'h':
			usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	divf = modf(div, &divi);
	idiv = divi;
	idivf = divf * 1e3;
	idiv &= 0xfff;
	idivf &= 0xfff;
	printf("div %.2f divi %u.%u clk %d range %u\n", div, idiv, idivf, clk, range);

	if ((rv = vioopen(vi, viobase(2)) < 0))
		errx(1, "failed to map virtio: %s", strerror(errno));

	// set for pwm output to audio jack
	gpiosel(vi, 40, GPIO_ALT0);
	gpiosel(vi, 45, GPIO_ALT0);

	// set clock divider
	// all the clocks run much faster than the sampling rate of sound
	// so any divisor would work really, just have to set the range
	// correctly below
	vi->cm[CM_PWMDIV] = CM_PASSWORD | idiv << 12 | idivf;
	vi->cm[CM_PWMCTL] = CM_PASSWORD | CM_ENAB | clk;

	// setup pwm speed, this will set how fast we are flushing
	// the fifo, the rng register can be seen as the period of the signal
	// so we vary this to get the audio sampling rate, which we
	// want to map to around 44.1khz and 48khz
	vi->pwm[PWM_RNG1] = range;
	vi->pwm[PWM_RNG2] = range;
	vi->pwm[PWM_CTL] = PWM_USEF2 | PWM_PWEN2 | PWM_USEF1 | PWM_PWEN1 | PWM_CLRF1;

	// clear errors before beginning
	vi->pwm[PWM_STA] = PWM_BERR | PWM_RERR1 | PWM_WERR1;

	start = time(NULL);
	count = 0;
	while (!done) {
		vi->pwm[PWM_FIF1] = rand() & 0xffff;
		count++;
		while (vi->pwm[PWM_STA] & 0x1)
			;

		// try to calculate the speed we are running at
		// per second by keeping a counter for every input
		// we put into the buffer, as the pwm will try to empty
		// it at the rate it is running at the range register is
		// the thing that determines this setting the clock divider
		// should not affect this because the clock divider on
		// the clock it is using is running faster than the range we set,
		// so it is bounded by the range register

		// if we want it to output at a frequency, we should run this
		// for one second to figure out the frequency we are running at
		// then we could use that to figure what value to set the range
		if (time(NULL) - start >= 1) {
			printf("%x %lu %lu\n", range, count, count * range);
			start = time(NULL);
			count = 0;
		}
	}

	vioclose(vi);

	return 0;
}
