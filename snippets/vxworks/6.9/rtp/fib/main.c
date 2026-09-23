#include <stdio.h>
#include <stdlib.h>
#include "fib.h"

int
main(int argc, char *argv[])
{
	int n;

	if (argc < 2) {
		fprintf(stderr, "fib <n>\n");
		return 1;
	}

	n = atoi(argv[1]);
	printf("fib(%d) = %d\n", n, fib(n));

	return 0;
}
