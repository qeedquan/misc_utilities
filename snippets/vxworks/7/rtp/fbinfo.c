#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <fbdev.h>

void
usage(void)
{
	fprintf(stderr, "usage: <device>\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	FB_IOCTL_ARG fbi;
	int fd;

	if (argc < 2)
		usage();

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		err(1, "open");

	ioctl(fd, FB_IOCTL_DEV_SHOW, 100);

	memset(&fbi, 0, sizeof(fbi));
	ioctl(fd, FB_IOCTL_GET_VIDEO_MODE, &fbi);
	printf("Video Mode: %s\n", fbi.getVideoMode.pBuf);

	memset(&fbi, 0, sizeof(fbi));
	ioctl(fd, FB_IOCTL_GET_FB_INFO, &fbi);
	printf("FB Info\n");
	printf("  First FB:  virt %p phys %p\n", fbi.getFbInfo.pFirstFb, fbi.getFbInfo.pFirstFbPhys);
	printf("  FB:        virt %p phys %p\n", fbi.getFbInfo.pFb, fbi.getFbInfo.pFbPhys);
	printf("  Dimension: %ux%u-%u\n", fbi.getFbInfo.width, fbi.getFbInfo.height, fbi.getFbInfo.bpp);
	printf("  Vsync:     %u\n", fbi.getFbInfo.vsync);
	printf("  Buffers:   %u\n", fbi.getFbInfo.buffers);

	close(fd);
	return 0;
}
