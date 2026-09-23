#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <err.h>

int
main(int argc, char *argv[])
{
	if (argc < 2)
		errx(1, "usage: word ...");

	srand(time(NULL));
	for (int i = 1; i < argc; i++) {
		printf("%s -> ", argv[i]);
		printf("%s\n", strfry(argv[i]));
	}
	return 0;
}
