#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <err.h>

int
main(void)
{
	char buf[8192];
	int fd, i;
	ssize_t nr;

	fd = memfd_create("memfd_file", MFD_CLOEXEC | MFD_ALLOW_SEALING);
	if (fd < 0)
		err(1, "memfd");

	if (ftruncate(fd, 1 * 1024 * 1024) < 0)
		err(1, "ftruncate");

	for (i = 0; i < 10; i++)
		dprintf(fd, "hello %d!\n", i);
	lseek(fd, 0, SEEK_SET);

	while ((nr = read(fd, buf, sizeof(buf))) > 0)
		write(1, buf, nr);
	close(fd);

	return 0;
}
