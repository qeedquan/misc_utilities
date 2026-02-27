// https://en.wikipedia.org/wiki/Stooge_sort
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

void
stoogesort_rec(int *a, size_t i, size_t j)
{
	int t;

	if (a[i] > a[j]) {
		t = a[j];
		a[j] = a[i];
		a[i] = t;
	}

	if ((j - i + 1) > 2) {
		t = (j - i + 1) / 3;
		stoogesort_rec(a, i, j - t);
		stoogesort_rec(a, i + t, j);
		stoogesort_rec(a, i, j - t);
	}
}

void
stoogesort(int *a, size_t n)
{
	if (n == 0)
		return;

	stoogesort_rec(a, 0, n - 1);
}

void
arand(int *a, size_t n)
{
	size_t i;
	for (i = 0; i < n; i++)
		a[i] = rand();
}

void
print(int *a, size_t n)
{
	size_t i;
	for (i = 0; i < n; i++) {
		printf("%d\n", a[i]);
	}
	printf("\n");
}

int
main(void)
{
	int a[5];

	srand(time(NULL));
	arand(a, nelem(a));
	print(a, nelem(a));
	stoogesort(a, nelem(a));
	print(a, nelem(a));
	return 0;
}
