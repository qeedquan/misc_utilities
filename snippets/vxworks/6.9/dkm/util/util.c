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
#include <sockLib.h>

extern SOCK_FUNC *sockLibMap[AF_MAX];

void
dumpsocklibmap(void)
{
	const char *protos[AF_MAX] = {
		[AF_UNSPEC] = "AF_UNSPEC",
		[AF_LOCAL] = "AF_LOCAL",
		[AF_INET] = "AF_INET",
		[AF_NETLINK] = "AF_NETLINK",
		[AF_ROUTE] = "AF_ROUTE",
		[AF_LINK] = "AF_LINK",
		[AF_PACKET] = "AF_PACKET",
		[AF_INET6] = "AF_INET6",
		[AF_KEY] = "AF_KEY",
		[AF_MPLS] = "AF_MPLS",
		[AF_SOCKDEV] = "AF_SOCKDEV",
		[AF_TIPC] = "AF_TIPC",
		[AF_MIPC] = "AF_MIPC",
		[AF_MIPC_SAFE] = "AF_MIPC_SAFE",
	};

	int i;

	printf("SOCKLIBMAP:\n");
	for (i = 0; i < AF_MAX; i++) {
		if (protos[i])
			printf("%16s", protos[i]);
		else
			printf("%16s", "AF_UNKNOWN");
		printf("\t%p\n", sockLibMap[i]);
	}
	printf("\n");
}

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
ioctlfile(const char *name, int op, int arg)
{
	STATUS status;
	int fd;

	status = OK;
	fd = open(name, O_RDONLY, 0);
	if (fd < 0) {
		printf("failed to open device %s: %s\n", name, strerror(errno));
		goto error;
	}

	if (ioctl(fd, op, arg) < 0) {
		printf("failed to issue ioctl: %s\n", strerror(errno));
		goto error;
	}

	if (0) {
	error:
		status = ERROR;
	}

	if (fd >= 0)
		close(fd);

	return status;
}

STATUS
remount(const char *devname)
{
	return ioctlfile(devname, XBD_HARD_EJECT, XBD_BASE);
}

STATUS
eject(const char *devname)
{
	return ioctlfile(devname, XBD_SOFT_EJECT, XBD_BASE);
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

void
hexdump(void *buf, size_t len)
{
	char *ptr;
	size_t i;

	printf("buf=%p, len=%lu\n", buf, (ULONG)len);
	ptr = buf;
	for (i = 0; i < len; i++) {
		printf("%02x ", ptr[i] & 0xff);
		if ((i & 15) == 15)
			printf("\n");
	}
	printf("\n");
}

void
xbddone(struct bio *bio)
{
	semGive(bio->bio_caller1);
}

STATUS
readblk(device_t dev, int blkno)
{
	struct bio bio;
	unsigned blksize;
	STATUS status;

	printf("%s(blkno=%d)\n", __func__, blkno);

	status = xbdBlockSize(dev, &blksize);
	if (status != OK) {
		printf("failed to get block size\n");
		goto error;
	}

	memset(&bio, 0, sizeof(bio));
	bio.bio_data = bio_alloc(dev, 1);
	bio.bio_caller1 = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
	if (!bio.bio_data || bio.bio_caller1 == SEM_ID_NULL)
		goto error;

	bio.bio_dev = dev;
	bio.bio_blkno = blkno;
	bio.bio_bcount = blksize;
	bio.bio_error = OK;
	bio.bio_flags = BIO_READ;
	bio.bio_done = xbddone;
	bio.bio_priority = 0;

	xbdStrategy(dev, &bio);

	if (semTake(bio.bio_caller1, WAIT_FOREVER) != OK) {
		printf("failed to read block data\n");
		goto error;
	}
	if (bio.bio_resid != 0 || bio.bio_error != OK) {
		printf("block read returned error\n");
		goto error;
	}

	hexdump(bio.bio_data, blksize);

	if (0) {
	error:
		status = ERROR;
	}

	bio_free(bio.bio_data);
	semDelete(bio.bio_caller1);

	return ERROR;
}

STATUS
fsblks(char *devname, int blkstart, int blkend)
{
	STATUS status;
	device_t dev;
	int blkno;

	status = OK;
	dev = devGetByName(devname);
	if (!dev) {
		printf("failed to get device\n");
		goto error;
	}

	for (blkno = blkstart; blkno <= blkend; blkno++)
		readblk(dev, blkno);

	if (0) {
	error:
		status = ERROR;
	}

	return status;
}

int
xbdinfo(const char *name)
{
	devname_t devname;
	XBD_GEOMETRY geo;
	XBD_INFO info;
	int attached;
	int devtype;
	int fd;

	fd = open(name, O_RDONLY, 0644);
	if (fd < 0)
		return -errno;

	memset(&geo, 0, sizeof(geo));
	memset(&info, 0, sizeof(info));
	devname[0] = '\0';
	devtype = -1;

	ioctl(fd, XBD_GETGEOMETRY, &geo);
	ioctl(fd, XBD_GETBASENAME, devname);
	attached = ioctl(fd, XBD_TEST);
	ioctl(fd, XBD_GETINFO, &info);
	ioctl(fd, XBD_GET_DEVTYPE, &devtype);

	printf("Device Name: %s\n", devname);
	printf("Device Type: %d\n", devtype);
	printf("Attached: %d\n", attached >= 0);
	printf("\n");

	printf("Geometry Info\n");
	printf("Heads: %u\n", geo.heads);
	printf("Cylinders: %llu\n", geo.cylinders);
	printf("Total Blocks: %lld\n", (long long)geo.total_blocks);
	printf("Sectors Per Tracks: %u\n", geo.secs_per_track);
	printf("Block Size: %u\n", geo.blocksize);
	printf("\n");

	close(fd);
	return 0;
}
