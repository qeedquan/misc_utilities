#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

void
usage(void)
{
	fprintf(stderr, "usage: <file>\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	char rbuf[512];
	char wbuf[] = "hello";
	int fd;
	ssize_t n;

	if (argc < 2)
		usage();

	fd = open(argv[1], O_RDWR | O_CREAT, 0644);
	if (fd < 0)
		err(1, "open");

	if (lseek(fd, 300, SEEK_SET) < 0)
		warn("lseek1");

	if ((n = read(fd, rbuf, sizeof(rbuf))) < 0)
		warn("read");
	printf("read %zd\n", n);

	if (lseek(fd, 512, SEEK_SET) < 0)
		warn("lseek2");

	if ((n = write(fd, wbuf, sizeof(wbuf))) < 0)
		warn("write");
	printf("write %zd\n", n);

	close(fd);
	return 0;
}
