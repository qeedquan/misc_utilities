#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

int
main(void)
{
	srand(time(NULL));

	static const void *ops[] = {&&a, &&b, &&c, &&d};
	for (int ticks = 0;; ticks++) {
		goto *ops[rand() % nelem(ops)];

	a:
		printf("a\n");
		continue;
	b:
		printf("b\n");
		continue;
	c:
		printf("c\n");
		continue;
	d:
		printf("d\n");
		if (ticks >= 32)
			break;
	}
	return 0;
}
