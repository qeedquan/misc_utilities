#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

double
arithmetic_incremental(double x0, double x1, size_t n)
{
	return x0 + (x1 - x0) / n;
}

double
arithmetic(double *v, size_t n)
{
	size_t i;
	double m;

	if (n == 0)
		return 0;

	m = 0;
	for (i = 0; i < n; i++)
		m += v[i];
	return m / n;
}

double
geometric(double *v, size_t n)
{
	size_t i;
	double m;

	if (n == 0)
		return 0;

	m = 1;
	for (i = 0; i < n; i++)
		m *= v[i];
	return pow(m, 1.0 / n);
}

double
variance(double *x, size_t n)
{
	double xm, s;
	size_t i;

	if (n < 2)
		return 0;

	xm = arithmetic(x, n);
	s = 0;
	for (i = 0; i < n; i++)
		s += (x[i] - xm) * (x[i] - xm);
	return s / (n - 1);
}

double
covariance(double *x, double *y, size_t n)
{
	double xm, ym, s;
	size_t i;

	if (n < 2)
		return 0;

	xm = arithmetic(x, n);
	ym = arithmetic(y, n);
	s = 0.0;
	for (i = 0; i < n; i++)
		s += (x[i] - xm) * (y[i] - ym);
	return s / (n - 1);
}

void
randv(double *v, size_t n, double s)
{
	size_t i;

	for (i = 0; i < n; i++)
		v[i] = drand48() * s;
}

void
addv(double *v, size_t n, double s)
{
	size_t i;

	for (i = 0; i < n; i++)
		v[i] += s;
}

// https://www.cut-the-knot.org/pythagoras/corollary.shtml
// for positive (a,b) (a+b)/2 >= sqrt(ab)
// (a+b)/2 is known as the arithmetic mean of the numbers a and b; sqrt(a*b) is their geometric mean also known as the mean proportional because if k=sqrt(a*b) then a/k=k/b and vice versa.
void
test_pythagorean_inequality(void)
{
	size_t i, j;

	for (i = 0; i < 1000; i++) {
		for (j = 0; j < 1000; j++) {
			double v[] = {i, j};
			assert(arithmetic(v, 2) >= geometric(v, 2));
		}
	}
}

// https://www.kalmanfilter.net/alphabeta.html
// we can incrementally update the average and get the same solution by using the identity
// x[n] = 1/N * sum(z[n]) = x[n-1] + 1/N*(z[n] - x[n-1])
// Since this is a recursive definition,
// N will take on the values 1, 2, 3 ... sizeof(array)
// this is useful if we don't want to keep a big buffer for all the history to calculate the average
void
test_arithmetic_incremental(void)
{
	double v[1000];
	double x, y;
	size_t i, n;

	for (n = 0; n < 10000; n++) {
		randv(v, nelem(v), n);
		x = arithmetic(v, nelem(v));
		y = v[0];
		for (i = 1; i < nelem(v); i++)
			y = arithmetic_incremental(y, v[i], i + 1);
		assert(fabs(x - y) < 1e-10);
	}
}

// https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
// x[n] = x[n-1] + (z[n]-x[n-1]) / n
// y[n] = y[n-1] + (w[n]-y[n-1]) / n
// C[n] = C[n-1] + (z[n]-x[n-1])*(w[n]-y[n])
// Cov = C[n] / (n-1)
void
test_covariance_incremental(void)
{
	double X[1000], Y[1000];
	double xi, yi, ci;
	double xn, yn;
	double u, v;
	size_t i, n;

	for (n = 2; n <= 100; n++) {
		randv(X, nelem(X), n);
		randv(Y, nelem(Y), n);

		xi = yi = ci = 0;
		for (i = 0; i < n; i++) {
			xn = arithmetic_incremental(xi, X[i], i + 1);
			yn = arithmetic_incremental(yi, Y[i], i + 1);
			ci += (X[i] - xi) * (Y[i] - yn);
			xi = xn;
			yi = yn;
		}
		u = covariance(X, Y, n);
		v = ci / (n - 1);

		assert(fabs(u - v) < 1e-10);
	}
}

/*

Given the arithmetic mean calculated on a set of values
If we add a constant to every value in the set, the mean calculated will shift by that constant.
The variance however, does not change under a shift.

*/

void
test_arithmetic_shift(void)
{
	static const double eps = 1e-8;

	double X[1000];
	size_t n;
	double m0, m1, x;
	double v0, v1;

	for (n = 1; n <= 1000; n++) {
		randv(X, nelem(X), n);
		x = drand48() * 1000;
		m0 = arithmetic(X, n);
		v0 = variance(X, n);

		addv(X, n, x);
		m1 = arithmetic(X, n);
		v1 = variance(X, n);

		assert(fabs(fabs(m1 - m0) - x) < eps);
		assert(fabs(v1 - v0) < eps);
	}
}

int
main(void)
{
	srand48(time(NULL));
	test_pythagorean_inequality();
	test_arithmetic_incremental();
	test_covariance_incremental();
	test_arithmetic_shift();
	return 0;
}
