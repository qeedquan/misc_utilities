/*

https://webaudio.github.io/Audio-EQ-Cookbook/audio-eq-cookbook.html
https://www.earlevel.com/main/2021/09/02/biquad-calculator-v3/

*/

#define _GNU_SOURCE
#include <stdio.h>
#include <math.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

typedef void (*filter_f)(double, double, double, double[3], double[3]);

void
bqp(double fs, double f0, double Q, double gain, double *w0, double *alpha, double *A)
{
	*w0 = 2 * M_PI * (f0 / fs);
	*alpha = sin(*w0) / (2 * Q);
	*A = pow(10, gain / 40);
}

void
bqlpf(double w0, double alpha, double, double z[3], double p[3])
{
	z[0] = (1 - cos(w0)) / 2;
	z[1] = 1 - cos(w0);
	z[2] = z[0];

	p[0] = 1 + alpha;
	p[1] = -2 * cos(w0);
	p[2] = 1 - alpha;
}

void
bqhpf(double w0, double alpha, double, double z[3], double p[3])
{
	z[0] = (1 + cos(w0)) / 2;
	z[1] = -(1 + cos(w0));
	z[2] = z[0];

	p[0] = 1 + alpha;
	p[1] = -2 * cos(w0);
	p[2] = 1 - alpha;
}

void
bqnotch(double w0, double alpha, double, double z[3], double p[3])
{
	z[0] = 1;
	z[1] = -2 * cos(w0);
	z[2] = 1;

	p[0] = 1 + alpha;
	p[1] = -2 * cos(w0);
	p[2] = 1 - alpha;
}

void
bqbpf_z(double w0, double alpha, double, double z[3], double p[3])
{
	z[0] = alpha;
	z[1] = 0;
	z[2] = -alpha;

	p[0] = 1 + alpha;
	p[1] = -2 * cos(w0);
	p[2] = 1 - alpha;
}

void
bqapf(double w0, double alpha, double, double z[3], double p[3])
{
	z[0] = 1 - alpha;
	z[1] = -2 * cos(w0);
	z[2] = 1 + alpha;

	p[0] = 1 + alpha;
	p[1] = -2 * cos(w0);
	p[2] = 1 - alpha;
}

void
bqpeak(double w0, double alpha, double A, double z[3], double p[3])
{
	z[0] = 1 + (alpha * A);
	z[1] = -2 * cos(w0);
	z[2] = 1 - (alpha * A);

	p[0] = 1 + (alpha / A);
	p[1] = -2 * cos(w0);
	p[2] = 1 - (alpha / A);
}

void
bqlowshelf(double w0, double alpha, double A, double z[3], double p[3])
{
	z[0] = A * ((A + 1) - ((A - 1) * cos(w0)) + (2 * sqrt(A) * alpha));
	z[1] = (2 * A) * ((A - 1) - ((A + 1) * cos(w0)));
	z[2] = A * ((A + 1) - ((A - 1) * cos(w0)) - (2 * sqrt(A) * alpha));

	p[0] = (A + 1) + ((A - 1) * cos(w0)) + (2 * sqrt(A) * alpha);
	p[1] = -2 * ((A - 1) + ((A + 1) * cos(w0)));
	p[2] = (A + 1) + ((A - 1) * cos(w0)) - (2 * sqrt(A) * alpha);
}

void
bqhighshelf(double w0, double alpha, double A, double z[3], double p[3])
{
	z[0] = A * ((A + 1) + ((A - 1) * cos(w0)) + (2 * sqrt(A) * alpha));
	z[1] = (-2 * A) * ((A - 1) + ((A + 1) * cos(w0)));
	z[2] = A * ((A + 1) + ((A - 1) * cos(w0)) - (2 * sqrt(A) * alpha));

	p[0] = (A + 1) - ((A - 1) * cos(w0)) + (2 * sqrt(A) * alpha);
	p[1] = 2 * ((A - 1) - ((A + 1) * cos(w0)));
	p[2] = (A + 1) - ((A - 1) * cos(w0)) - (2 * sqrt(A) * alpha);
}

void
bqnormalize(double z[3], double p[3])
{
	int i;
	double s;

	s = p[0];
	for (i = 0; i < 3; i++) {
		z[i] /= s;
		p[i] /= s;
	}
}

void
test_filter(double fs, double f0, double Q, double gain, int normalize)
{
	static const char *names[] = {
	    "Low Pass",
	    "High Pass",
	    "Band Pass (0 dB) peak gain",
	    "Notch",
	    "All Pass",
	    "Peaking Equalizer",
	    "Low Shelf",
	    "High Shelf",
	};

	static const filter_f filters[] = {
	    bqlpf,
	    bqhpf,
	    bqbpf_z,
	    bqnotch,
	    bqapf,
	    bqpeak,
	    bqlowshelf,
	    bqhighshelf,
	};

	double w0, alpha, A;
	double z[3], p[3];
	size_t i;

	for (i = 0; i < nelem(filters); i++) {
		bqp(fs, f0, Q, gain, &w0, &alpha, &A);
		filters[i](w0, alpha, A, z, p);
		if (normalize)
			bqnormalize(z, p);

		printf("%s Filter\n", names[i]);
		printf("zeroes: %.12f %.12f %.12f\n", z[0], z[1], z[2]);
		printf("poles:  %.12f %.12f %.12f\n", p[0], p[1], p[2]);
		printf("\n");
	}
}

int
main(void)
{
	test_filter(233, 100, 0.7071, 3, 1);

	return 0;
}
