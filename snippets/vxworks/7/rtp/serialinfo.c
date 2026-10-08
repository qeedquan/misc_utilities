#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <sioLib.h>

void
serialinfo(const char *name)
{
	char ident[64];
	int availmodes, mode, baudrate, hwopts;
	int fd;

	fd = open(name, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "stderr: failed to open serial device: %s\n", strerror(errno));
		return;
	}

	ident[0] = '\0';
	baudrate = -1;
	mode = -1;
	availmodes = -1;
	hwopts = -1;

	ioctl(fd, SIO_NAME_GET, ident);
	ioctl(fd, SIO_BAUD_GET, &baudrate);
	ioctl(fd, SIO_MODE_GET, &mode);
	ioctl(fd, SIO_AVAIL_MODES_GET, &availmodes);
	ioctl(fd, SIO_HW_OPTS_GET, &hwopts);
	
	printf("Device %s\n", name);
	printf("Identifier:       %s\n", ident);
	printf("Baud Rate:        %d\n", baudrate);
	printf("Mode:             %#x\n", mode);
	printf("Available Modes:  %#x\n", availmodes);
	printf("HW Options:       %#x\n", hwopts);
	printf("\n");

	close(fd);
}

void
usage(void)
{
	errx(2, "usage: <device> ...\n");
}

int
main(int argc, char *argv[])
{
	int i;

	if (argc < 2)
		usage();

	for (i = 1; i < argc; i++)
		serialinfo(argv[i]);

	return 0;
}
