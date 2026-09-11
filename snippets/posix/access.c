#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

void
usage(void)
{
	fprintf(stderr, "usage: file ...\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	int i;

	if (argc < 2)
		usage();
	for (i = 1; i < argc; i++) {
		printf("%s: ", argv[i]);
		if (access(argv[i], F_OK) < 0)
			printf("%s\n", strerror(errno));
		else
			printf("access ok\n");
	}
	return 0;
}
