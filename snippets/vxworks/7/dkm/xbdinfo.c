#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <drv/xbd/xbd.h>
#include <vxbSataLib.h>

int
xbdinfo(const char *name)
{
	devname_t devname;
	XBD_GEOMETRY geo;
	XBD_INFO info;
	ATA_IDENTIFY_INFO ata;
	int attached;
	int devtype;
	int fd;

	fd = open(name, O_RDONLY);
	if (fd < 0)
		return -errno;

	memset(&geo, 0, sizeof(geo));
	memset(&info, 0, sizeof(info));
	memset(&ata, 0, sizeof(ata));
	devname[0] = '\0';
	devtype = -1;

	ioctl(fd, XBD_GETGEOMETRY, &geo);
	ioctl(fd, XBD_GETBASENAME, devname);
	attached = ioctl(fd, XBD_TEST);
	ioctl(fd, XBD_GETINFO, &info);
	ioctl(fd, XBD_GET_DEVTYPE, &devtype);
	ioctl(fd, XBD_SATA_PARMSGET, &ata);

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

	printf("ATA Info:\n");
	printf("Serial: %.*s\n", sizeof(ata.serial), ata.serial);
	printf("Revision: %*.s\n", sizeof(ata.rev), ata.rev);
	printf("Model: %*.s\n", sizeof(ata.model), ata.model);
	printf("Cylinders: %d\n", ata.cylinders);
	printf("Heads: %d\n", ata.heads);
	printf("Bytes Per Sector: %d\n", ata.bytesSec);
	printf("Sector Buffer Size (In Sectors): %d\n", ata.size);
	printf("ECC Bytes: %d\n", ata.bytesEcc);
	printf("PIO Mode: %d\n", ata.pioMode);
	printf("DMA Mode: %d\n", ata.dmaMode);
	printf("Capacity 0: %d\n", ata.capacity0);
	printf("Capacity 1: %d\n", ata.capacity1);
	printf("Sectors 0: %d\n", ata.sectors0);
	printf("Sectors 1: %d\n", ata.sectors1);
	printf("Queue Depth: %d\n", ata.queueDepth);

	close(fd);
	return 0;
}
