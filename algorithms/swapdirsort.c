#include <stdio.h>
#include <limits.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

void
swap(int *a, int *b)
{
	int t;

	t = *a;
	*a = *b;
	*b = t;
}

void
sdsortrec(int *a, size_t n, size_t i, size_t d)
{
	if (n < 2)
		return;
	if (a[i + 1] < a[i]) {
		swap(&a[i + 1], &a[i]);
		if (i > 0)
			sdsortrec(a, n, i - 1, 0);
	}
	if (i + 2 < n && d == 1)
		sdsortrec(a, n, i + 1, 1);
}

void
sdsort(int *a, size_t n)
{
	sdsortrec(a, n, 0, 1);
}

void
print(int *a, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++)
		printf("%d ", a[i]);
	printf("\n");
}

void
test(int *a, size_t n)
{
	sdsort(a, n);
	print(a, n);
}

int
main(void)
{
	int a1[] = {4, -3, 6, 8, 2, 7};
	test(a1, nelem(a1));

	int a2[] = {1398, -6, 24, -1, 13, 65, 24, -490, -2000};
	test(a2, nelem(a2));

	int a3[] = {INT_MAX, INT_MIN};
	test(a3, nelem(a3));
}
