// http://iq0.com/notes/vdiv.html
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

typedef struct {
	double x, y, z;
} vec;

double
vdot(vec a, vec b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec
vcross(vec a, vec b)
{
	vec v;
	v.x = a.y * b.z - a.z * b.y;
	v.y = a.z * b.x - a.x * b.z;
	v.z = a.x * b.y - a.y * b.x;
	return v;
}

double
vlen(vec a)
{
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

vec
vadd(vec a, vec b)
{
	return (vec){a.x + b.x, a.y + b.y, a.z + b.z};
}

vec
vsub(vec a, vec b)
{
	return (vec){a.x - b.x, a.y - b.y, a.z - b.z};
}

vec
vscale(vec a, double s)
{
	return (vec){a.x * s, a.y * s, a.z * s};
}

vec
vneg(vec a)
{
	return vscale(a, -1);
}

double
vquo(vec a, vec b)
{
	return vdot(a, b) / vdot(b, b);
}

vec
vmod(vec a, vec b)
{
	return vsub(a, vscale(b, vquo(a, b)));
}

vec
vunit(vec a)
{
	double l = vlen(a);
	return (vec){a.x / l, a.y / l, a.z / l};
}

vec
varot(vec r, vec a, double t)
{
	vec v1 = vscale(a, vquo(r, a));
	vec v2 = vscale(vmod(r, a), cos(t));
	vec v3 = vcross(vunit(a), vscale(vmod(r, a), sin(t)));
	return vadd(v1, vadd(v2, v3));
}

const char *
vstr(vec a)
{
	static char buf[8][80];
	static size_t bp = 0;

	bp &= 7;
	snprintf(buf[bp], sizeof(buf[bp]), "{%.2lf, %.2lf, %.2lf}", a.x, a.y, a.z);
	return buf[bp++];
}

vec
vrand(void)
{
	return (vec){drand48(), drand48(), drand48()};
}

void
identities(void)
{
	// (a+b)/c  = a/c + b/c
	vec a = vrand();
	vec b = vrand();
	vec c = vrand();
	double r1 = vquo(vadd(a, b), c);
	double r2 = vquo(a, c) + vquo(b, c);
	printf("%lf %lf\n", r1, r2);

	// (a+b) mod c = a mod c + b mod c
	a = vrand();
	b = vrand();
	c = vrand();
	vec v1 = vmod(vadd(a, b), c);
	vec v2 = vadd(vmod(a, c), vmod(b, c));
	printf("%s %s\n", vstr(v1), vstr(v2));

	// (a mod b) dot b = 0
	a = vrand();
	b = vrand();
	r1 = vdot(vmod(a, b), b);
	printf("%lf\n", r1);

	// a/a = 1
	a = vrand();
	printf("%lf\n", vquo(a, a));

	// -a mod b = -(a mod -b)
	a = vrand();
	b = vrand();
	v1 = vmod(vneg(a), b);
	v2 = vneg(vmod(a, vneg(b)));
	printf("%s %s\n", vstr(v1), vstr(v2));

	// rotation about axis
	v1 = (vec){1, 0, 0};
	v2 = (vec){0, 1, 0};
	vec v3 = varot(v1, v2, M_PI / 2);
	printf("%s\n", vstr(v3));
}

int
main(void)
{
	srand48(time(NULL));
	identities();
	return 0;
}
