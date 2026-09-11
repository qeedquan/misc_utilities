// https://basesandframes.files.wordpress.com/2020/04/even_faster_math_functions_green_2020.pdf
#include <stdio.h>
#include <math.h>

float
rsqrt(float x)
{
	union {
		int i;
		float f;
	} u;
	u.f = x;
	u.i = 0x5f37624f - (u.i >> 1);
	return u.f;
}

int
main(void)
{
	unsigned i;
	float x, y, z;

	for (i = 0; i <= 10000; i++) {
		x = 1 / sqrt(i);
		y = rsqrt(i);
		z = fabs(x - y);
		printf("%.6f %.6f %.6f\n", x, y, z);
	}

	return 0;
}
