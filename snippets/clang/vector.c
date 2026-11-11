#include <stdio.h>

typedef int int2 __attribute__((ext_vector_type(2)));
typedef int int3 __attribute__((ext_vector_type(3)));
typedef int int4 __attribute__((ext_vector_type(4)));
typedef int float2 __attribute__((ext_vector_type(2)));
typedef int float3 __attribute__((ext_vector_type(3)));
typedef int float4 __attribute__((ext_vector_type(4)));

typedef float3 mat3[3];
typedef float4 mat4[4];

int
main(void)
{
	int3 x = { 1, 2, 3 };
	int3 y = { 4, 5, 6 };
	x += y;
	printf("%d %d %d\n", x[0], x[1], x[2]);

	x *= x;
	printf("%d %d %d\n", x.x, x.y, x.z);

	int2 z = x.xy;
	printf("%d %d\n", z.r, z.g);

	float4 x4 = { 1, 2, 3, 4 };
	float4 y4 = { 5, 6, 7, 8 };
	mat4 m4;
	m4[0] = x4;
	m4[1] = y4;
	printf("%d %d %d\n", m4[0][0], m4[0][1], m4[0][2]);
	printf("%d %d %d\n", m4[1][0], m4[1][1], m4[1][2]);
	printf("%zu\n", sizeof(m4));

	return 0;
}
