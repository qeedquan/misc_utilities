#include <stdio.h>
#include <immintrin.h>

int
main(void)
{
	long long tsc;

	for (;;) {
		tsc = _rdtsc();
		printf("%lld\n", tsc);
	}
	return 0;
}
