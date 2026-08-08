#include <stdio.h>

// Can't set this past some number, native arithmetic ops stops working because it becomes unimplemented (get compiler error when try to use arithmetic ops)
typedef _ExtInt(128) int128_t;
typedef unsigned _ExtInt(128) uint128_t;

void
print(uint128_t x)
{
	int d[256], i, n;

	printf("sizeof: %zu\n", sizeof(x));

	n = 0;
	do {
		d[n++] = x % 10;
	} while (x /= 10);

	for (i = n - 1; i >= 0; i--)
		printf("%d", d[i]);
	printf("\n");
}

int
main(void)
{
	uint128_t x;

	x = 0;
	print(~x);

	return 0;
}
