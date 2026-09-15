/*

We want to uniformly sample over an interval [t2, t3] that is a subinterval of [t0, t1]  with step size s

The uniform interval [t0, t1] with step size s
-  - - - -
t0 s s s t1

The uniform subinterval [t2, t3] inside [t0, t1] with step size s
-  - t2 - - - t3 -
t0 s    s s s    t1

The solution is to pick a starting point where t2 starts to closest sampling point that is sampled
within [t0, t1] and then start stepping until t3 is reached

*/

#include <assert.h>
#include <stdio.h>
#include <math.h>

double
sample(double t0, double t1, double t2, double t3, double s)
{
	double t;

	assert(t0 <= t2 && t2 <= t1);
	assert(t0 <= t3 && t3 <= t1);

	printf("[t0, t1]\n");
	for (t = t0; t <= t1; t += s)
		printf("%.3f ", t);
	printf("\n");

	// f(x) = t0 + x*s
	// g(x) = t2
	// minimize f(x) - g(x)
	// while ensuring x is inside [t2, t3]
	// x = ceil((t2 - t0) / s)
	// t = t0 + s*x
	printf("[t2, t3]\n");
	t = ceil((t2 - t0) / s);
	for (t = t0 + s * t; t <= t3; t += s)
		printf("%.3f ", t);
	printf("\n");

	return 0;
}

int
main(void)
{
	sample(2, 3, 2.51, 2.8, 1 / 16.0);
	return 0;
}
