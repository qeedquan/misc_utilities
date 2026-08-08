#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

void
usage(void)
{
	fprintf(stderr, "usage: <mode> <file>\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	struct flock lock;
	struct flock info;
	const char *mode;
	FILE *fp;
	int fd;

	if (argc != 3)
		usage();

	memset(&lock, 0, sizeof(lock));
	mode = "rb";
	switch (argv[1][0]) {
	case 'r':
		lock.l_type = F_RDLCK;
		break;
	case 'w':
		mode = "wb";
		lock.l_type = F_WRLCK;
		break;
	case 'u':
		lock.l_type = F_UNLCK;
		break;
	default:
		errx(1, "unknown mode: %s", argv[1]);
		break;
	}

	fp = fopen(argv[2], mode);
	if (!fp)
		err(1, "open");

	fd = fileno(fp);

	memset(&info, 0, sizeof(info));
	if (fcntl(fd, F_GETLK, &info) < 0)
		errx(1, "failed to get lock info: %s", strerror(errno));

	if (fcntl(fd, F_SETLK, &lock) < 0)
		errx(1, "failed to set lock info: %s", strerror(errno));

	fclose(fp);

	return 0;
}
