#include <stdio.h>
#include <stdlib.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

int
clamp(int x, int a, int b)
{
	if (x < a)
		x = a;
	if (x > b)
		x = b;
	return x;
}

int
geturand(void)
{
	FILE *fp;
	int r;

	fp = fopen("/dev/urandom", "rb");
	if (!fp)
		return -1;

	r = fgetc(fp) & 0xff;
	fclose(fp);
	return r;
}

void
dist(int mask, int iters)
{
	int h[256] = {0};
	int i, j;

	for (i = 0; i < iters; i++) {
		j = geturand();
		if (j < 0)
			continue;
		h[j % mask]++;
	}
	for (i = 0; i < mask; i++)
		printf("%d %d\n", i, h[i]);
}

int
main(int argc, char *argv[])
{
	int iters = 10;
	int mask = 2;
	if (argc >= 2)
		mask = atoi(argv[1]);
	if (argc >= 3)
		iters = atoi(argv[2]);
	mask = clamp(mask, 0, 255);
	dist(mask, iters);
	return 0;
}
