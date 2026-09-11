// https://en.wikipedia.org/wiki/Z-order_curve

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

// http://graphics.stanford.edu/~seander/bithacks.html#InterleaveTableObvious
unsigned
morton(unsigned x, unsigned y)
{
	unsigned long z;
	size_t i;

	z = 0;
	for (i = 0; i < sizeof(x) * CHAR_BIT; i++)
		z |= (x & 1U << i) << i | (y & 1U << i) << (i + 1);
	return z;
}

typedef struct {
	unsigned x, y, z;
} Point;

Point
mkpt(unsigned x, unsigned y)
{
	return (Point){x, y, morton(x, y)};
}

int
cmppt(const void *a, const void *b)
{
	Point *p, *q;

	p = (Point *)a;
	q = (Point *)b;
	if (p->z < q->z)
		return -1;
	if (p->z > q->z)
		return 1;
	return 0;
}

#define N 50

int
main(void)
{
	static Point pt[N * N];
	unsigned i, j;

	for (i = 0; i < N; i++) {
		for (j = 0; j < N; j++)
			pt[i * N + j] = mkpt(j, i);
	}
	qsort(pt, N * N, sizeof(*pt), cmppt);

	// the points are close to monotonically increasing in length
	// if we sort by their morton ordering
	for (i = 0; i < N * N; i++) {
		printf("(%u, %u): %u %u %lf\n",
		       pt[i].x, pt[i].y, pt[i].z,
		       pt[i].x + pt[i].y, hypot(pt[i].x, pt[i].y));
	}
}
