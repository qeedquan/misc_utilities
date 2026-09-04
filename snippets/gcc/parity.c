#define _GNU_SOURCE
#include <stdio.h>

int
paritykr(unsigned i)
{
	int p;

	for (p = 0; i; i &= (i - 1))
		p = !p;
	return p;
}

int
parity8(unsigned v)
{
	int i, c;
	for (i = c = 0; i < 8; i++)
		c += ((v >> i) & 1);
	return !((c & 1) == 0);
}

int
main(void)
{
	unsigned i, r[3];
	for (i = 0; i < 0x100; i++) {
		r[0] = __builtin_parity(i);
		r[1] = paritykr(i);
		r[2] = parity8(i);
		if (r[0] != r[1] || r[1] != r[2])
			printf("%d %d %d %d\n", i, r[0], r[1], r[2]);
	}
}
