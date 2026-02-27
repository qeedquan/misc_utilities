// given 2 interval (x1, x2) and (y1, y2)
// determine if they overlap
#include <stdio.h>

void
swapf(float *x, float *y)
{
	float t;

	t = *x;
	*x = *y;
	*y = t;
}

int
segintf(float x1, float x2, float y1, float y2)
{
	if (x1 > x2)
		swapf(&x1, &x2);
	if (y1 > y2)
		swapf(&y1, &y2);
	return x2 >= y1 && y2 >= x1;
}

int
main(void)
{
	float x1, y1, x2, y2;
	float N = 10.0f;

	for (x1 = -N; x1 <= N; x1++) {
		for (y1 = -N; y1 <= N; y1++) {
			for (x2 = -N; x2 <= N; x2++) {
				for (y2 = -N; y2 <= N; y2++) {
					printf("%.2f %.2f %.2f %.2f %d\n",
					       x1, x2, y1, y2, segintf(x1, x2, y1, y2));
				}
			}
		}
	}
	return 0;
}
