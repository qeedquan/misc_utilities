#include <assert.h>
#include <stdalign.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include <smmintrin.h>

int
main(void)
{
	alignas(128) float f[4];
	__m128 x, y, z, s;

	__builtin_cpu_init();
	assert(__builtin_cpu_supports("sse4.2"));
	x = _mm_set_ps(1.0f, 4.0f, 9.0f, 16.0f);
	y = _mm_sqrt_ps(x);
	s = _mm_set1_ps(2);
	z = _mm_mul_ps(y, s);
	z = _mm_rcp_ps(z);

	memcpy(f, &z, sizeof(z));
	printf("%f %f %f %f\n", f[0], f[1], f[2], f[3]);

	z = _mm_rcp_ps(z);
	z = _mm_dp_ps(x, x, 0xff);
	memcpy(f, &z, sizeof(z));
	printf("%f %f %f %f\n", f[0], f[1], f[2], f[3]);

	return 0;
}
