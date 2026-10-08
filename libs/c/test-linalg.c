#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "linalg.h"

void
test_transform(void)
{
	float4 v = {{1, 2, 3, 4}};
	float4x4 M = {
	    {1, 2, 3, 4},
	    {5, 6, 7, 8},
	    {9, 10, 11, 12},
	    {13, 14, 15, 16},
	};

	// M*transpose(v)
	float4 p = trf4x4(M, v);
	// v*M
	float4 q = trb4x4(v, M);

	printf("Testing Transform\n");
	mfdump(&p, 1, 4);
	mfdump(&q, 1, 4);
}

void
test_matrix_multiplication(void)
{
	float4x4 A = {
	    {50, 38, 19, 49},
	    {94, 28, 20, 58},
	    {96, 10, 86, 1},
	    {-5, -2, -6, -4},
	};
	float4x4 B = {
	    {1, 2, 3, 4},
	    {5, 6, 7, 8},
	    {9, 10, 11, 12},
	    {13, 14, 15, 16},
	};
	float4x4 C = {
	    {4, 1, 6, 1},
	    {19, 58, 7, -2},
	    {-6, 2, -4, 1},
	    {9, 7, 9, 1},
	};
	float4x4 D = {
	    {3, 5, 6, 2},
	    {29, 659, 1, 5},
	    {-21, -76, -2, 1},
	    {1, 6, 2, 3},
	};
	float4x4 M, N;

	// A*B*C*D
	mul4x4(M, C, D);
	mul4x4(M, B, M);
	mul4x4(M, A, M);

	mul4x4(N, A, B);
	mul4x4(N, N, C);
	mul4x4(N, N, D);

	printf("Testing Matrix Multiplication\n");
	mfdump(M, 4, 4);
	mfdump(N, 4, 4);
}

void
test_quaternion_matrix(void)
{
	printf("Testing quaternion matrix\n");

	float4x4 R;
	rotate4x3(R, (float3){{6, 2.5, 3}}, M_PI / 4);

	float4 q = qrotate4x4(R);
	float4x4 S;
	qmat4x4(S, q);

	mfdump(R, 4, 4);
	mfdump(S, 4, 4);
}

void
test_outer_product(void)
{
	printf("Testing outer product\n");

	// (a ⊗ b)*c = a*(b.c)
	float4x4 m;
	float4 a = {{2, 1, -1, 0}};
	float4 b = {{1, 2, 1, 0}};
	float4 c = {{-1, 1, 1, 0}};
	outer4(m, a, b);
	float4 d = trf4x4(m, c);
	float4 e = scale4(a, dot4(b, c));
	mfdump(&d, 1, 4);
	mfdump(&e, 1, 4);
}

void
test_coordinates(void)
{
	printf("Testing Spherical coordinates\n");
	float3 x = V3(1, 2, 3);
	float3 y = xyzsph(x);
	float3 z = sphxyz(y);
	printf("%f %f %f %f %f %f\n", x.x, x.y, x.z, z.x, z.y, z.z);

	printf("Testing Cylindrical coordinates\n");
	y = xyzcyl(x);
	z = cylxyz(y);
	printf("%f %f %f %f %f %f\n", x.x, x.y, x.z, z.x, z.y, z.z);
}

void
test_frustum(void)
{
	printf("Testing Symmetric Frustum\n");
	// points at the length (z*l, z*r, z, 1) gets mapped to (+-n, +-n, z', 1)
	// points at the length (z*l*f/n, z*r*f/n, z, 1) gets mapped to (+-f, +-f, z', 1)
	for (int i = 0; i < 10; i++) {
		double l = -100 * drand48();
		double t = -100 * drand48();
		double n = 5 * drand48();
		double f = 1000 * drand48();

		double r = -l;
		double b = -t;
		float4x4 m;
		frustum4x4(m, l, r, b, t, n, f);

		double z = -drand48() * 100;
		float4 p = trf4x4(m, V4(z * l, z * t, z, 1));
		float4 q = trf4x4(m, V4(l, t, z, 1));
		p = scale4(p, 1 / p.w);
		q = scale4(q, 1 / q.w);
		printf("left top near %f z=%f | %f %f %f %f | %f %f %f %f\n", n, z, p.x, p.y, p.z, p.w, q.x, q.y, q.z, q.w);

		z = -drand48() * 1000;
		p = trf4x4(m, V4(z * r, z * b, z, 1));
		q = trf4x4(m, V4(r, b, z, 1));
		p = scale4(p, 1 / p.w);
		q = scale4(q, 1 / q.w);
		printf("right bottom near %f z=%f | %f %f %f %f | %f %f %f %f\n", n, z, p.x, p.y, p.z, p.w, q.x, q.y, q.z, q.w);

		z = -drand48() * 100;
		p = trf4x4(m, V4(f * z * l / n, z * f * t / n, z, 1));
		q = trf4x4(m, V4(f * z * r / n, z * f * b / n, z, 1));
		p = scale4(p, 1 / p.w);
		q = scale4(q, 1 / q.w);
		printf("left top near %f far %f z=%f | %f %f %f %f\n", n, f, z, p.x, p.y, p.z, p.w);
		printf("right bottom near %f far %f z=%f | %f %f %f %f\n", n, f, z, q.x, q.y, q.z, q.w);
		printf("\n");

		// staring straight down the z axis gives us
		// the projection n=1, f=-1 as n goes to f
		// 1 goes to -1 but not linearly, the rate of change is related to the ratio f/n
		// if f/n is large then 1 to -1 takes giant steps at the beginning and move slowly
		// if f/n is small then 1 to -1 are moved at smaller increments
		printf("straight down z moving at n/f speeds\n");
		double u = n;
		double lz = 1;
		while (u <= f) {
			p = trf4x4(m, V4(0, 0, -u, 1));
			p = scale4(p, 1 / p.w);
			printf("%f far/near %f near/far %f z/lz %f |z-lz| %f | %f %f %f %f\n",
			       u, f / n, n / f, p.z / lz, fabs(p.z - lz), p.x, p.y, p.z, p.w);
			lz = p.z;
			u += n / f;
		}
	}
}

void
test_roots(void)
{
	float r[16];
	int s = roots2rf(10, 145, 13, r);
	printf("Testing roots\n");
	printf("%d %f %f\n", s, r[0], r[1]);
	s = roots2rf(1, 13, 13, r);
	printf("%d %f %f\n", s, r[0], r[1]);
	s = roots2rf(100, 134, 13, r);
	printf("%d %f %f\n", s, r[0], r[1]);

	s = roots3rf(1, 45, 24, 42, r);
	printf("%d %f\n", s, r[0]);

	s = roots3rf(52, 145, 524, 342, r);
	printf("%d %f\n", s, r[0]);

	s = roots3rf(140, 15, 5, 4, r);
	printf("%d %f\n", s, r[0]);

	s = roots3rf(4, 0, 0, 57, r);
	printf("%d %f\n", s, r[0]);

	s = roots3rf(1, -6, 11, -6, r);
	printf("%d %f %f %f\n", s, r[0], r[1], r[2]);
}

void
test_quaternion_sqrt(void)
{
	printf("Testing quaternion sqrt\n");

	float4 q = V4(1, 2, 3, 4);
	float4 r = qsqrt(q);
	float4 p = qmul(r, r);
	mfdump(&q, 4, 1);
	mfdump(&r, 4, 1);
	mfdump(&p, 4, 1);
}

void
test_polar_decomposition_2x2(void)
{
	for (int i = 0; i < 1000; i++) {
		float2x2 M, R, S, Rd, Sd;
		
		rotate2x2(R, drand48() * 2 * M_PI);
		scale2x2(S, V2(drand48() * 100, drand48() * 100));
		mul2x2(M, R, S);

		decompose2x2(M, Rd, Sd);

		assert(mfclose(R, Rd, 2, 2, 1e-4));
		assert(mfclose(S, Sd, 2, 2, 1e-4));
	}
}

int
main(void)
{
	srand48(time(NULL));
	test_polar_decomposition_2x2();
	test_transform();
	test_matrix_multiplication();
	test_quaternion_matrix();
	test_outer_product();
	test_coordinates();
	test_frustum();
	test_roots();
	test_quaternion_sqrt();
	
	return 0;
}
