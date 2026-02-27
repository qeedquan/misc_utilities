#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <math.h>

#define TAU (2 * M_PI)

// slow way to reduce x to [-pi, pi)
double
mod2pi(double x)
{
	double y;

	y = x;
	while (y >= M_PI)
		y -= TAU;
	while (y < -M_PI)
		y += TAU;
	return y;
}

double
mod2pifast(double x)
{
	double y;

	y = fmod(x, TAU);
	if (x < 0 && y != 0)
		y += TAU;

	if (y >= M_PI)
		y -= TAU;

	return y;
}

int
main(void)
{
	static const double eps = 1e-3;
	static const double step = 1e-2;
	static const double N = 5000;

	double x, y0, y1;

	for (x = -N * TAU; x <= N * TAU; x += step) {
		y0 = mod2pifast(x);
		y1 = mod2pi(x);
		assert(fabs(y0 - y1) < eps);
	}

	return 0;
}
