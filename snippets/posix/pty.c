#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

int
main(void)
{
	int fd;

	fd = posix_openpt(O_RDWR);
	if (fd < 0)
		err(1, "posix_openpt");

	if (grantpt(fd) < 0)
		err(1, "grantpt");

	if (unlockpt(fd) < 0)
		err(1, "unlockpt");

	printf("ptsname: %s\n", ptsname(fd));
	printf("isatty:  %d\n", isatty(fd));

	close(fd);

	return 0;
}
