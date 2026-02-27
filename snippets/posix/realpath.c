#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int
main(int argc, char *argv[])
{
	char path[PATH_MAX];
	int i;

	for (i = 1; i < argc; i++) {
		realpath(argv[i], path);
		printf("%s\n", path);
	}
	return 0;
}
