// https://en.wikipedia.org/wiki/Maximum_subarray_problem
#include <stdio.h>
#include <limits.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

int
kadane(int *a, size_t n)
{
	size_t i;
	int r;

	if (n == 0)
		return INT_MIN;

	for (i = 1; i < n; i++) {
		if (a[i - 1] > 0)
			a[i] += a[i - 1];
	}

	r = a[0];
	for (i = 1; i < n; i++) {
		if (r < a[i])
			r = a[i];
	}
	return r;
}

int
main(void)
{
	int v1[] = {-2, 1, -3, 4, -1, 2, 1, -5, 4};
	printf("%d\n", kadane(v1, nelem(v1)));
	return 0;
}
