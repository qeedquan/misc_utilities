// https://lauri.võsandi.com/hdl/arithmetic/goldschmidt-division-algorithm.html
// https://en.wikipedia.org/wiki/Division_algorithm#Goldschmidt_division

#include <assert.h>
#include <stdio.h>
#include <math.h>

double
goldschmidt(double n, double d, int t)
{
	double f;
	int i;

	f = 1 / d;
	for (i = 0; i < t; i++) {
		n = f * n;
		d = f * d;
		f = 2 - d;
	}
	return n;
}

int
main(void)
{
	double N;
	double a, b, s;
	double x, y;

	N = 1e4;
	s = 1e-1;
	for (a = -N; a <= N; a += s) {
		for (b = -N; b <= N; b += s) {
			x = a / b;
			y = goldschmidt(a, b, 4);
			assert(fabs(x - y) < 1e-3);
		}
	}
	return 0;
}
