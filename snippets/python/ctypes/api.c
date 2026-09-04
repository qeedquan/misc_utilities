#include <stdio.h>
#include <string.h>

typedef struct {
	float x, y, z;
} Point;

typedef struct {
	int x, y, z;
	float fx, fy, fz;
	char s1[64];
	char s2[32];
	Point pt[10];
	float (*fp)[80];
	int nfp;
} Value;

typedef struct {
	char a[3];
	short b;
	int c;
} __attribute__((packed)) Packed;

float FP1[80] = { 1, 2, 3 };
float (*FP)[80] = &FP1;

void
fill(Value *v, size_t n)
{
	size_t i;
	int j;

	for (i = 0; i < n; i++) {
		v->x = i;
		v->y = i + 2;
		v->z = i + 3;
		v->fx = 3.14 * i;
		v->fy = 2.81 * i;
		v->fz = 305.53 * i;
		strcpy(v->s1, "hello");
		strcpy(v->s2, "world");
		for (j = 0; j < 10; j++)
			v->pt[j] = (Point){ j, j, j };
		v->fp = FP;
		v->nfp = 3;
		printf("%p\n", v->fp);
		v++;
	}
}

void
show(Value *v, size_t n)
{
	size_t i;
	int j;

	printf("\nShow\n");
	for (i = 0; i < n; i++) {
		printf("%f %f %f\n", (*v[i].fp)[0], (*v[i].fp)[1], (*v[i].fp)[2]);
	}
	printf("\n");
}

void
packed(Packed *p)
{
	printf("%zu\n", sizeof(*p));
	printf("%d %d %d\n", p->a[0], p->a[1], p->a[2]);
	printf("%d\n", p->b);
	printf("%d\n", p->c);
}

void
points(Point *p, size_t n)
{
	size_t i;

	for (i = 0; i < n; i++) {
		printf("%f %f %f\n", p[i].x, p[i].y, p[i].z);
	}
}