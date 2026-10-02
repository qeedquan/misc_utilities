/*

http://dspfirst.gatech.edu/chapters/DSP1st2eLabs/BPFnullingFilters.pdf
https://dsp.stackexchange.com/questions/36499/nulling-filter-coefficients
https://tomroelandts.com/articles/why-use-symmetrical-fir-filters-with-an-odd-length
https://tomroelandts.com/articles/the-phase-response-of-a-filter
https://tomroelandts.com/articles/applying-a-filter-in-both-directions-makes-it-zero-phase

We generally want FIR coefficients as opposed to IIR because of linear phase (no phase distortion, all frequencies are shifted by the same amount of phase),
and they are BIBO (bounded input, bounded output, they don't blow up if we have bounded input; IIR can blow up on bounded input in some cases)
The downsides is that they are more computationally expensive than IIR since they require more coefficients to do the achieve the same response.

Also want the coefficients to be odd length because it makes the delay have an integer number of samples.
This means that the original signal can be compared with the filtered one by shifting samples (don't need a fractional delay filter).

The formula for determining the delay is
d = (N-1)/2 where N is the number of samples and d is number of samples it is delayed by.

If we apply any filter in both directions, the combined filtering operation is zero phase.
The way to do this is:

1. Filter the input signal x[n]. This results in an intermediate signal xi[n].
2. Reverse the order of the samples in xi[n].
3. Filter xi[n] with the same filter. This results in the output signal y[n].
4. Reverse the order of the samples in y[n] to get the final output signal.

The problem is that this is not casual anymore, a casual filter cannot have zero phase.

*/

#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>
#include <fftw3.h>

#define TAU (2 * M_PI)
#define rad2deg(x) (((x)*180) / M_PI)

// attenuate at a specific frequency w
void
null(double a[3], double w)
{
	a[0] = 1;
	a[1] = -2 * cos(w * 2 * M_PI);
	a[2] = 1;
}

double
filter(double *x, double *a, size_t n)
{
	double r;
	size_t i;

	r = 0;
	for (i = 0; i < n; i++)
		r += x[i] * a[i];
	return r;
}

void
gencos(double *a, size_t n, size_t m, double s)
{
	double t, dt;
	size_t i, j;

	if (n == 0)
		return;

	t = 0;
	dt = 1.0 / n;
	for (i = 0; i < n; i++) {
		a[i] = 0;
		for (j = 0; j <= m; j++)
			a[i] += cos(2 * M_PI * t * j);
		a[i] *= s;

		t += dt;
	}
}

void
dumpf(double *a, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++)
		printf("%.6f\n", a[i]);
	printf("\n");
}

void
dumpc(double complex *a, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++) {
		printf("%zu %.6f %.6f | %.6f %.6f\n",
		       i,
		       creal(a[i]), cimag(a[i]),
		       cabs(a[i]), rad2deg(carg(a[i])));
	}
	printf("\n");
}

void
testnull(size_t n)
{
	fftw_plan plan;
	double complex *fft;
	double *in, *firin, *out;
	double fir[3];
	double omega;
	size_t ntaps;
	size_t i;

	ntaps = 3;
	in = calloc(n, sizeof(*in));
	firin = calloc(n + ntaps, sizeof(*firin));
	out = calloc(n, sizeof(*out));
	fft = calloc(n / 2 + 1, sizeof(*fft));
	assert(in);
	assert(firin);
	assert(out);
	assert(fft);

	// omega is normalized frequency
	for (omega = 0; omega < n / 2; omega++) {
		printf("omega = %f [%f %f %f]\n", omega, fir[0], fir[1], fir[2]);
		null(fir, omega / n);
		gencos(in, n, n / 2, 10);

		memset(firin, 0, sizeof(*firin) * (n + ntaps));
		memcpy(firin + ntaps, in, sizeof(*in) * n);

		for (i = 0; i < n; i++)
			out[i] = filter(firin + i + 1, fir, ntaps);

		plan = fftw_plan_dft_r2c_1d(n, out, fft, FFTW_ESTIMATE);
		fftw_execute(plan);
		for (i = 0; i < n / 2; i++) {
			fft[i] /= n;
			if (i)
				fft[i] *= 2;
		}
		dumpc(fft, n / 2);

		fftw_destroy_plan(plan);
	}

	free(in);
	free(firin);
	free(out);
	free(fft);
}

int
main(void)
{
	testnull(32);
	return 0;
}
