// https://en.wikipedia.org/wiki/Bilinear_interpolation
#include <stdio.h>

double
lerp(double t, double a, double b)
{
	return a + (b - a) * t;
}

double
blerp(double c00, double c10, double c01, double c11, double tx, double ty)
{
	return lerp(lerp(c00, c10, tx), lerp(c01, c11, tx), ty);
}

int
main(void)
{
	double i, j, k, l, s;
	double tx, ty;

	s = 0.05;
	tx = ty = 1;
	for (i = 0; i <= 1; i += s) {
		for (j = 0; j <= 1; j += s) {
			for (k = 0; k <= 1; k += s) {
				for (l = 0; l <= 1; l += s) {
					printf("[%.2f,%.2f,%.2f,%.2f] %f\n", i, j, k, l, blerp(i, j, k, l, tx, ty));
				}
			}
		}
	}
	return 0;
}
