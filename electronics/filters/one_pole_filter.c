// https://www.earlevel.com/main/2012/12/15/a-one-pole-filter/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

typedef double (*fn_t)(double, void *);

typedef struct {
	double a0, b1, z1;
} opft_t;

// fc is the normalized frequency
// fc = frequency / sample_rate
void
opft_setlpf(opft_t *o, double fc)
{
	o->b1 = exp(-2.0 * M_PI * fc);
	o->a0 = 1.0 - o->b1;
}

void
opft_sethpf(opft_t *o, double fc)
{
	o->b1 = -exp(-2.0 * M_PI * (0.5 - fc));
	o->a0 = 1.0 + o->b1;
}

double
opft_process(opft_t *o, float v)
{
	o->z1 = v * o->a0 + o->z1 * o->b1;
	return o->z1;
}

typedef struct {
	double t0, t1;
	double dc, nl;
} delta_fn_t;

double
delta(double t, void *u)
{
	delta_fn_t *d;

	d = u;
	if (d->t0 <= t && t <= d->t1)
		return d->dc + d->nl * drand48();
	return 0;
}

typedef struct {
	double dc;
	unsigned hm;
} wave_fn_t;

double
wave(double t, void *u)
{
	wave_fn_t *w;
	unsigned i;
	double r;

	w = u;
	r = 0;
	for (i = 1; i <= w->hm; i++) {
		r += w->dc * sin(2 * M_PI * t / i) / i;
	}
	return r;
}

void
test_filter(opft_t *o, double t0, double t1, double dt, fn_t f, void *u)
{
	double t, x, y, dcb;

	for (t = t0; t <= t1; t += dt) {
		x = f(t, u);
		y = opft_process(o, x);

		// for dc blocker, subtract every sample with filtered value
		dcb = x - y;

		printf("%.4f %.6f %.6f %.6f\n", t, x, y, dcb);
	}
}

int
main(void)
{
	srand(time(NULL));
	srand48(time(NULL));

	// for a delta function a one-pole filter
	// acts like a exponential decay, the first harmonic
	// is considered the dc so set fc to that
	if (1) {
		opft_t o1;
		delta_fn_t f1 = { 0, 0, 100, 0 };
		opft_setlpf(&o1, 1.0 / 40);
		test_filter(&o1, 0, 200, 0.025, delta, &f1);
	}

	if (1) {
		opft_t o2;
		wave_fn_t f2 = { 1000, 50 };
		opft_setlpf(&o2, 1.0 / 100.0);
		test_filter(&o2, 0, 200, 1 / 100.0, wave, &f2);
	}

	return 0;
}
