#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int
main(int argc, char *argv[])
{
	int fd;
	char *p;

	fd = 0;
	if (argc >= 2)
		fd = atoi(argv[1]);

	p = ttyname(fd);
	if (!p) {
		fprintf(stderr, "ttyname: %s\n", strerror(errno));
		return 0;
	}

	printf("%s\n", p);
	return 1;
}
