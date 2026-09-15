// https://en.wikipedia.org/wiki/Simple_polygon
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct {
	double x, y;
} Point;

void *
xcalloc(size_t nmemb, size_t size)
{
	void *p;

	p = calloc(nmemb, size);
	if (!p)
		abort();
	return p;
}

Point *
randpts(size_t n)
{
	Point *p;
	size_t i;

	p = xcalloc(n, sizeof(*p));
	for (i = 0; i < n; i++)
		p[i] = (Point){drand48() * 1e4, drand48() * 1e4};
	return p;
}

// can re-arrange the shoelace formula to get this
// 0.5 * summation( (x[i+1] + x[i])*(y[i+1] - y[i]) )
double
area1(Point *p, size_t n)
{
	double s;
	size_t i;

	if (n < 3)
		return 0;

	s = 0;
	for (i = 0; i < n - 1; i++)
		s += (p[i + 1].x + p[i].x) * (p[i].y - p[i + 1].y);
	s += (p[0].x + p[n - 1].x) * (p[n - 1].y - p[0].y);

	return fabs(s * 0.5);
}

// https://en.wikipedia.org/wiki/Shoelace_formula
double
area2(Point *p, size_t n)
{
	double s1, s2;
	size_t i;

	if (n < 3)
		return 0;

	s1 = 0;
	s2 = 0;
	for (i = 0; i < n - 1; i++) {
		s1 += p[i].x * p[i + 1].y;
		s2 += p[i].y * p[i + 1].x;
	}

	s1 += p[n - 1].x * p[0].y;
	s2 += p[0].x * p[n - 1].y;
	return fabs(s1 - s2) * 0.5;
}

int
main(void)
{
	Point *p;
	size_t i, N;

	srand48(time(NULL));
	N = 10000;
	for (i = 1; i <= N; i++) {
		p = randpts(i);
		printf("%zu %lf %lf\n", i, area1(p, i), area2(p, i));
		free(p);
	}
	return 0;
}
