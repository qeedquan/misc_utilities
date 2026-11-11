// cc -o rdrand rdrand.c -march=native
#include <stdio.h>
#include <immintrin.h>

int
main(void)
{
	unsigned short r16;
	unsigned int r32;
	unsigned long long r64;

	r16 = 0;
	r32 = 0;
	r64 = 0;
	for (;;) {
		_rdrand16_step(&r16);
		_rdrand32_step(&r32);
		_rdrand64_step(&r64);
		printf("%x %x %llx\n", r16, r32, r64);
	}
	return 0;
}
