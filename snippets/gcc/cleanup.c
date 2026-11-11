#include <stdio.h>

// calls function to cleanup when variable goes out of scope
#define CLEAN(f) __attribute__((__cleanup__(f)))

void
fint(int *x)
{
	printf("%s(%d)\n", __func__, *x);
}

void
fflt(float *x)
{
	printf("%s(%f)\n", __func__, *x);
}

void
test(void)
{
	int a CLEAN(fint);
	float b CLEAN(fflt);

	a = 1;
	b = 2;
	{
		int c CLEAN(fint);
		float d CLEAN(fflt);

		c = 3;
		d = 4;

		{
			int x CLEAN(fint), y CLEAN(fint);

			x = 5;
			y = 6;
		}
	}
}

int
main(void)
{
	test();
	return 0;
}
