#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <vxWorks.h>
#include <tyLib.h>
#include <dosFsLib.h>
#include <xbdRamDisk.h>

void
showstatfs(char *name)
{
	struct statfs64 fs;
	long total, freed;

	if (statfs64(name, &fs) < 0) {
		fprintf(stderr, "%s: %s\n", name, strerror(errno));
		return;
	}

	total = fs.f_bsize * fs.f_blocks;
	freed = fs.f_bsize * fs.f_bfree;
	if (total < 0)
		total = 0;
	if (freed < 0)
		freed = 0;

	printf("%s\n", name);
	printf("type:              %lx\n", fs.f_type);
	printf("block size:        %ld\n", fs.f_bsize);
	printf("total blocks:      %ld\n", (long)fs.f_blocks);
	printf("free blocks:       %ld\n", (long)fs.f_bfree);
	printf("available blocks:  %ld\n", (long)fs.f_bavail);
	printf("total file nodes:  %ld\n", (long)fs.f_files);
	printf("free file nodes:   %ld\n", (long)fs.f_ffree);

	printf("\n");
	printf("total size in bytes:     %ld\n", total);
	printf("total size in megabytes: %f\n", total / (1024.0 * 1024.0));

	printf("\n");
	printf("free size in bytes:         %ld\n", freed);
	printf("free size in megabytes:     %f\n", freed / (1024.0 * 1024.0));
	printf("\n");
}

STATUS
dosformat(const char *devname, size_t size)
{
	size_t blocksize = 512;

	if (xbdRamDiskDevCreate(blocksize, size, FALSE, devname) == NULLDEV) {
		printf("failed to create ram disk '%s'\n", devname);
		return ERROR;
	}

	if (dosFsVolFormat(devname, DOS_OPT_BLANK, NULL) != OK) {
		printf("failed to format ram disk\n");
		return ERROR;
	}

	return OK;
}

STATUS
setecho(int fd, int enable)
{
	int options;

	options = ioctl(fd, FIOGETOPTIONS, 0);
	if (options < 0)
		return ERROR;

	if (enable)
		options |= OPT_ECHO;
	else
		options &= ~OPT_ECHO;

	return ioctl(fd, FIOSETOPTIONS, options);
}

unsigned
ipchecksum(void *addr, int len)
{
	unsigned char *ptr;
	unsigned sum;

	sum = 0;
	ptr = addr;
	for (sum = 0; len > 1; len -= 2) {
		sum += ptr[0] | (ptr[1] << 8);
		ptr += 2;
	}
	if (len > 0)
		sum += ptr[0];

	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	return ~sum & 0xFFFF;
}

STATUS
ipchecksumfile(const char *name, int blocksize)
{
	int nread;
	int block;
	int fd;
	char *data;
	STATUS status;

	status = OK;
	fd = -1;
	data = malloc(blocksize);
	if (!data)
		goto error;

	fd = open(name, O_RDONLY, 0);
	if (fd < 0)
		goto error;

	block = 0;
	for (;;) {
		nread = read(fd, data, blocksize);
		if (nread <= 0)
			break;

		printf("block %d (%d bytes): checksum %#x\n", block, nread, ipchecksum(data, nread));
		block += 1;
	}

	if (0) {
	error:
		status = ERROR;
	}

	if (fd >= 0)
		close(fd);

	free(data);

	return status;
}
