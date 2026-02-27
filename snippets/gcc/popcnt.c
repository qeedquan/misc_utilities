#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>

#define B2(n) n, n + 1, n + 1, n + 2
#define B4(n) B2(n), B2(n + 1), B2(n + 1), B2(n + 2)
#define B6(n) B4(n), B4(n + 1), B4(n + 1), B4(n + 2)
static const char table[256] = {
    B6(0),
    B6(1),
    B6(1),
    B6(2),
};

int
popcnt0(uint64_t x)
{
	return table[(x & 0xff00000000000000ull) >> 0x38] +
	       table[(x & 0x00ff000000000000ull) >> 0x30] +
	       table[(x & 0x0000ff0000000000ull) >> 0x28] +
	       table[(x & 0x000000ff00000000ull) >> 0x20] +
	       table[(x & 0x00000000ff000000ull) >> 0x18] +
	       table[(x & 0x0000000000ff0000ull) >> 0x10] +
	       table[(x & 0x000000000000ff00ull) >> 0x08] +
	       table[(x & 0x00000000000000ffull) >> 0x00];
}

int
popcnt1(uint64_t x)
{
	return __builtin_popcount(x);
}

int
popcnt2(uint64_t x)
{
	int r;
	for (r = 0; x; x >>= 8)
		r += table[x & 0xFF];
	return r;
}

int
popcnt3(uint64_t x)
{
	size_t i;
	int r;
	for (r = i = 0; i < sizeof(x); x >>= 8, i++)
		r += table[x & 0xFF];
	return r;
}

int
popcnt4(uint64_t x2)
{
	uint32_t x;
	x2 = x2 - ((x2 >> 1) & 0x5555555555555555uLL);
	/* Every 2 bits holds the sum of every pair of bits (32) */
	x2 = ((x2 >> 2) & 0x3333333333333333uLL) + (x2 & 0x3333333333333333uLL);
	/* Every 4 bits holds the sum of every 4-set of bits (3 significant bits) (16) */
	x2 = (x2 + (x2 >> 4)) & 0x0F0F0F0F0F0F0F0FuLL;
	/* Every 8 bits holds the sum of every 8-set of bits (4 significant bits) (8) */
	x = (uint32_t)(x2 + (x2 >> 32));
	/* The lower 32 bits hold four 16 bit sums (5 significant bits). */
	/*   Upper 32 bits are garbage */
	x = x + (x >> 16);
	/* The lower 16 bits hold two 32 bit sums (6 significant bits). */
	/*   Upper 16 bits are garbage */
	return (x + (x >> 8)) & 0x0000007F; /* (7 significant bits) */
}

int
popcnt5(uint64_t x)
{
	int c;
	for (c = 0; x != 0; x &= x - 1)
		c++;
	return c;
}

void
test(const char *name, int (*popcnt)(uint64_t))
{
	struct timespec t0, t1;
	int sum0, i;

	timespec_get(&t0, TIME_UTC);
	sum0 = 0;
	srand(7777);
	for (i = 0; i < 10000000; i++)
		sum0 += popcnt(rand());
	timespec_get(&t1, TIME_UTC);

	printf("%s (%d) %f\n",
	       name,
	       sum0,
	       (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1.e9);
}

#define test(x) test(#x, x)

int
main(void)
{
	test(popcnt0);
	test(popcnt1);
	test(popcnt2);
	test(popcnt3);
	test(popcnt4);
	test(popcnt5);
	return 0;
}
