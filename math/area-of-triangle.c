#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

typedef struct {
	double x, y;
} Point;

// http://iq0.com/duffgram/invert33.c

/*
 * Compute the adjoint of a 3x3 matrix.
 * madj may alias m.
 */
void
adjoint33(double madj[3][3], double m[3][3])
{
	double m00, m01, m02, m10, m11, m12, m20, m21, m22;

	m00 = m[0][0];
	m01 = m[0][1];
	m02 = m[0][2];
	m10 = m[1][0];
	m11 = m[1][1];
	m12 = m[1][2];
	m20 = m[2][0];
	m21 = m[2][1];
	m22 = m[2][2];
	madj[0][0] = m11 * m22 - m12 * m21;
	madj[0][1] = m02 * m21 - m01 * m22;
	madj[0][2] = m01 * m12 - m02 * m11;
	madj[1][0] = m12 * m20 - m10 * m22;
	madj[1][1] = m00 * m22 - m02 * m20;
	madj[1][2] = m02 * m10 - m00 * m12;
	madj[2][0] = m10 * m21 - m11 * m20;
	madj[2][1] = m01 * m20 - m00 * m21;
	madj[2][2] = m00 * m11 - m01 * m10;
}

/*
 * Compute the inverse of a 3x3 matrix, returning the determinant.
 * If m is singular, minv is not touched.
 * minv may alias m.
 */
double
invert33(double minv[3][3], double m[3][3])
{
	double d;             /* determinant */
	double c00, c01, c02; /* cofactors of first row */
	double m00, m01, m02, m10, m11, m12, m20, m21, m22;

	m00 = m[0][0];
	m01 = m[0][1];
	m02 = m[0][2];
	m10 = m[1][0];
	m11 = m[1][1];
	m12 = m[1][2];
	m20 = m[2][0];
	m21 = m[2][1];
	m22 = m[2][2];
	c00 = m11 * m22 - m12 * m21;
	c01 = m12 * m20 - m10 * m22;
	c02 = m10 * m21 - m11 * m20;
	d = m00 * c00 + m01 * c01 + m02 * c02;
	if (d == 0.)
		return 0.;
	minv[0][0] = c00 / d;
	minv[1][0] = c01 / d;
	minv[2][0] = c02 / d;
	minv[0][1] = (m02 * m21 - m01 * m22) / d;
	minv[1][1] = (m00 * m22 - m02 * m20) / d;
	minv[2][1] = (m01 * m20 - m00 * m21) / d;
	minv[0][2] = (m01 * m12 - m02 * m11) / d;
	minv[1][2] = (m02 * m10 - m00 * m12) / d;
	minv[2][2] = (m00 * m11 - m01 * m10) / d;
	return d;
}

double
det33(double m[3][3])
{
	double d;             /* determinant */
	double c00, c01, c02; /* cofactors of first row */
	double m00, m01, m02, m10, m11, m12, m20, m21, m22;

	m00 = m[0][0];
	m01 = m[0][1];
	m02 = m[0][2];
	m10 = m[1][0];
	m11 = m[1][1];
	m12 = m[1][2];
	m20 = m[2][0];
	m21 = m[2][1];
	m22 = m[2][2];
	c00 = m11 * m22 - m12 * m21;
	c01 = m12 * m20 - m10 * m22;
	c02 = m10 * m21 - m11 * m20;
	d = m00 * c00 + m01 * c01 + m02 * c02;
	return d;
}

double
distance(Point p, Point q)
{
	return hypot(q.x - p.x, q.y - p.y);
}

Point
subpt(Point p, Point q)
{
	return (Point){
	    p.x - q.x,
	    p.y - q.y,
	};
}

Point
randpt(void)
{
	return (Point){
	    drand48() * 1e2,
	    drand48() * 1e2,
	};
}

// http://mathforum.org/library/drmath/view/55063.html
double
triareadet(Point p0, Point p1, Point p2)
{
	double m[3][3] = {
	    {p0.x, p0.y, 1},
	    {p1.x, p1.y, 1},
	    {p2.x, p2.y, 1},
	};
	return fabs(0.5 * det33(m));
}

double
triareaheron(Point p0, Point p1, Point p2)
{
	double a, b, c, s;

	a = distance(p0, p1);
	b = distance(p0, p2);
	c = distance(p1, p2);
	s = (a + b + c) * 0.5;
	return sqrt(s * (s - a) * (s - b) * (s - c));
}

double
triareacross(Point p0, Point p1, Point p2)
{
	Point p02, p12;
	double l;

	p02 = subpt(p2, p0);
	p12 = subpt(p2, p1);

	// cross product with 2d vectors will always result in another
	// vector that is of the form (0, 0, z) so the length is just abs(z)
	l = fabs(p02.x * p12.y - p02.y * p12.x);

	return 0.5 * l;
}

int
main(void)
{
	Point p[] = {
	    {1, 0},
	    {2, 34},
	    {3, 221},
	};
	printf("%lf %lf %lf\n",
	       triareadet(p[0], p[1], p[2]),
	       triareaheron(p[0], p[1], p[2]),
	       triareacross(p[0], p[1], p[2]));

	srand(time(NULL));
	for (;;) {
		p[0] = randpt();
		p[1] = randpt();
		p[2] = randpt();
		printf("%lf %lf %lf\n",
		       triareadet(p[0], p[1], p[2]),
		       triareaheron(p[0], p[1], p[2]),
		       triareacross(p[0], p[1], p[2]));
	}
	return 0;
}