#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

void
sample1(double *x, size_t n, double v)
{
	memmove(x + 1, x, (n - 1) * sizeof(*x));
	x[0] = v;
}

void
sample2(double *x, size_t n, double v, size_t *xp)
{
	if (++*xp >= n)
		*xp -= n;
	x[*xp] = v;
}

double
filter1(double *x, double *a, size_t n)
{
	double v;
	size_t i;

	v = 0.0;
	for (i = 0; i < n; i++)
		v += a[i] * x[i];
	return v;
}

double
filter2(double *x, double *a, size_t n, size_t xp)
{
	double v;
	size_t i, j;

	v = 0.0;
	j = xp;
	for (i = 0; i < n; i++) {
		v += a[i] * x[j];
		j = (j == 0) ? n - 1 : j - 1;
	}
	return v;
}

void *
xcalloc(size_t nmemb, size_t size)
{
	void *p;

	if (nmemb == 0)
		nmemb = 1;
	if (size == 0)
		size = 1;
	p = calloc(nmemb, size);
	if (!p)
		abort();
	return p;
}

void
adrand(double *p, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++)
		p[i] = drand48();
}

void
test(size_t n)
{
	double *a, *x1, *x2, v1, v2, r;
	size_t i, l, xp;

	a = xcalloc(n, sizeof(*a));
	x1 = xcalloc(n, sizeof(*x1));
	x2 = xcalloc(n, sizeof(*x2));
	xp = 0;

	adrand(a, n);

	l = n * 100;
	for (i = 0; i < l; i++) {
		r = drand48();

		sample1(x1, n, r);
		sample2(x2, n, r, &xp);

		v1 = filter1(x1, a, n);
		v2 = filter2(x2, a, n, xp);
		assert(fabs(v2 - v1) < 1e-6);
	}

	free(a);
	free(x1);
	free(x2);
}

int
main(void)
{
	srand48(time(NULL));
	test(1024);
	test(13);
	return 0;
}
