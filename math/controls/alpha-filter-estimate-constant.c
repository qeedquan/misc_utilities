// https://www.kalmanfilter.net/alphabeta.html
// Assume the gold has a constant weight, and we are making a noisy measurements
// Try to determine the gold weight
// This is a static model so we are trying to estimate a constant

#define _GNU_SOURCE
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

double
s_measure(void *ud)
{
	static const double tab[] = {
	    1030,
	    989,
	    1017,
	    1009,
	    1013,
	    979,
	    1008,
	    1042,
	    1012,
	    1011,
	};
	static size_t i;

	double r;

	r = tab[i];
	i = (i + 1) % nelem(tab);

	(void)ud;

	return r;
}

double
d_measure(void *ud)
{
	double r, s;

	r = *(double *)ud;
	s = (2 * drand48()) - 1;
	return r + (s * 10);
}

double
predict(double x, double a, double z)
{
	return x + a * (z - x);
}

void
test(double r, size_t n, double (*measure)(void *), void *ud)
{
	double a, x;
	size_t i;

	printf("iters %zu\n", n);
	x = measure(ud);
	for (i = 1; i <= n; i++) {
		printf("%zu r %.2f x %.2f err %.3f\n", i, r, x, fabs(r - x));

		// alpha is 1/1 1/2 1/3 1/4 ...
		// implying that at iteration N, we are implicitly returning the average
		// with N elements, but we are doing this memoryless instead of keeping a buffer
		// using the averaging identity
		// x[n] = 1/N*sum(z[1..n]) = x[n-1] * 1/N*(z[n]-x[n-1])

		// alpha can be thought of as a gain (how much the new information is worth to us)
		// since alpha gets smaller and smaller, we can think of it as each new information
		// we get isn't that valuable to us because we are estimating a constant here so the first
		// few iterations gets us most of the way there
		a = 1.0 / (i + 1);
		x = predict(x, a, measure(ud));
	}
	printf("\n");
}

int
main(void)
{
	double r;

	test(1010, 10, s_measure, NULL);
	test((r = 31468), 50, d_measure, &r);
	test((r = 34), 5, d_measure, &r);
	test((r = -4534), 10, d_measure, &r);

	return 0;
}
