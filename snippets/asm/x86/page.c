#include <stdio.h>

#define BY2PG 0x1000ull
#define PGSHIFT 12
#define PTSHIFT 9
#define PTLX(v, l) (((v) >> (((l)*PTSHIFT) + PGSHIFT)) & ((1 << PTSHIFT) - 1))
#define PGLSZ(l) (1ull << (((l)*PTSHIFT) + PGSHIFT))

int
main(void)
{
	unsigned long long i, va;

	for (i = 0; i < 4; i++)
		printf("%llx\n", PGLSZ(i));

	for (i = 1; i < 64; i++) {
		va = 0xffull << i;
		printf("%lld %llx\n", i, PTLX(va, 1));
	}

	return 0;
}
