/*

Given a function that is a linear combination of of sine and cosines

a0*cos(2pi*t) + a1*cos(2pi*2*t) + ...
b0*sin(2pi*t) + b1*sin(2pi*2*t) + ...

the generated wave can be fed into a fourier transform where the coefficients a[i] and b[i] can be recovered

*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <complex.h>
#include <getopt.h>
#include <fftw3.h>

#define rad2deg(x) ((x)*180 / M_PI)

typedef struct {
	double scale;
	size_t ncoeffs;
} Option;

Option opt = {
    .scale = 1000,
    .ncoeffs = 100,
};

void *
xcalloc(size_t nmemb, size_t size)
{
	void *p;

	if (nmemb == 0)
		nmemb = 1;
	if (size == 0)
		size = 1;

	p = calloc(nmemb, size);
	if (!p)
		abort();
	return p;
}

void
usage(void)
{
	fprintf(stderr, "usage: [options]\n");
	fprintf(stderr, "\t-h                print this message\n");
	fprintf(stderr, "\t-n (default: %zu) number of coefficients\n", opt.ncoeffs);
	fprintf(stderr, "\t-s (default: %f)  coefficients scale\n", opt.scale);
	exit(2);
}

void
parseopt(Option *o, int *argc, char ***argv)
{
	int c;

	while ((c = getopt(*argc, *argv, "hn:s:")) != -1) {
		switch (c) {
		case 'h':
			usage();
			break;

		case 'n':
			o->ncoeffs = atoi(optarg);
			break;

		case 's':
			o->scale = atof(optarg);
			break;
		}
	}

	*argc -= optind;
	*argv += optind;
}

double complex *
genpoly(size_t len, double scale, double init)
{
	double complex *a;
	size_t i;

	if (len == 0)
		return NULL;

	a = xcalloc(len, sizeof(*a));
	a[0] = init * scale;
	for (i = 1; i < len; i++)
		a[i] = drand48() * scale;
	return a;
}

double complex
eval(double complex *ca, double complex *sa, size_t n, double t)
{
	double complex r;
	size_t i;

	r = 0;
	for (i = 0; i < n; i++) {
		r += ca[i] * ccos(2 * M_PI * i * t);
		r += sa[i] * csin(2 * M_PI * i * t);
	}
	return r;
}

int
main(int argc, char *argv[])
{
	double complex *ca, *sa, *xs, *ys, phi;
	fftw_plan plan;
	size_t i, l;
	double re, im;
	double t;

	srand48(time(NULL));
	parseopt(&opt, &argc, &argv);

	ca = genpoly(opt.ncoeffs, opt.scale, 1);
	sa = genpoly(opt.ncoeffs, opt.scale, 0);

	l = opt.ncoeffs * 2;
	xs = xcalloc(l, sizeof(*xs));
	ys = xcalloc(l, sizeof(*ys));

	t = 0;
	for (i = 0; i < l; i++) {
		xs[i] = eval(ca, sa, opt.ncoeffs, t);
		t += 1.0 / l;
	}

	plan = fftw_plan_dft_1d(l, xs, ys, FFTW_FORWARD, FFTW_ESTIMATE);
	fftw_execute(plan);

	for (i = 0; i < l / 2; i++) {
		// this works because the fourier transform of pure sine/cosine waves are delta functions
		// fft(cos(2*pi*A*t)) = 0.5*(delta(f-A) + delta(f+A))
		// fft(sin(2*pi*A*t)) = 0.5*(delta(f-A) - delta(f+A))
		// the delta functions encode the amplitude at that frequency
		//
		// another way to think about it is the fourier transform gives a split complex oscillator for a real oscillator
		// complex oscillator + complex oscillator = real oscillator
		if (i == 0) {
			re = creal(ys[0]);
			im = cimag(ys[0]);
		} else {
			re = creal(ys[i] + ys[l - i]);
			im = cimag(-ys[i] + ys[l - i]);
		}
		re /= l;
		im /= l;
		phi = catan(ys[i]);

		printf("%f %f | %f %f | %f %f\n", creal(ca[i]), creal(sa[i]), re, im, rad2deg(creal(phi)), rad2deg(cimag(phi)));
	}

	fftw_destroy_plan(plan);
	free(ca);
	free(sa);
	free(xs);
	free(ys);

	return 0;
}
