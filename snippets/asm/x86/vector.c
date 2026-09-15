#include <stdio.h>
#include <immintrin.h>

typedef int v4si __attribute__((vector_size(16)));
typedef float v4sf __attribute__((vector_size(16)));
typedef double v4df __attribute__((vector_size(32)));
typedef unsigned long long v4di __attribute__((vector_size(32)));

void
ps4i(v4si x)
{
	printf("{%d, %d, %d, %d}\n", x[0], x[1], x[2], x[3]);
}

void
ex1(void)
{
	v4si a = {1, 2, 3, 4};
	v4si b = 2 * a;
	v4si c = 5 + b;
	v4si d = c > a;
	ps4i(a);
	ps4i(b);
	ps4i(c);
	ps4i(d);

	for (int i = 0; i < 8; i++) {
		v4si m1 = {0, i, i, 3};
		v4si m2 = {0, i, 2, i};

		v4si r1 = __builtin_shuffle(a, m1);
		v4si r2 = __builtin_shuffle(a, m2);

		ps4i(r1);
		ps4i(r2);
	}
}

void
ex2(void)
{
	v4si a = {1, -2, 3, -4};
	v4sf b = {1.5f, -2.5f, 3.f, 7.f};
	v4di c = {1ULL, 5ULL, 0ULL, 10ULL};
	(void)a;
	(void)b;
	(void)c;
}

int
main(void)
{
	ex1();
	ex2();
	return 0;
}
