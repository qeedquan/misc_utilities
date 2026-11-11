#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

void
usage(void)
{
	fprintf(stderr, "usage: path ...\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	char *p;
	int i;

	if (argc < 2)
		usage();

	for (i = 1; i < argc; i++) {
		p = realpath(argv[i], NULL);
		if (!p)
			printf("realpath: %s: %s\n", argv[i], strerror(errno));
		else
			printf("%s\n", p);
		free(p);
	}

	return 0;
}
