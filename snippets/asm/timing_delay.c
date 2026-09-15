#include <stdint.h>

void
delaygcc(uint64_t n)
{
	uint64_t i;

	// tell compiler not to optimize this out
	for (i = 0; i < n; i++)
		asm volatile("" ::: "memory");
}

int
main(void)
{
	delaygcc(10000000000ULL);
	return 0;
}
