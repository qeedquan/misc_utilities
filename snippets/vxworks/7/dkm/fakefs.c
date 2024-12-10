/*

Fake filesystem for testing APIs

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <semLib.h>
#include <drv/xbd/xbd.h>
#include <drv/xbd/bio.h>
#include <drv/erf/erfLib.h>
#include <fsEventUtilLib.h>
#include <fsMonitor.h>

void
hexdump(void *buf, size_t len)
{
	char *ptr;
	size_t i;

	printf("buf=%p, len=%zu\n", buf, len);
	ptr = buf;
	for (i = 0; i < len; i++) {
		printf("%02x ", ptr[i] & 0xff);
		if ((i & 15) == 15)
			printf("\n");
	}
	printf("\n");
}

void
ffsxbddone(struct bio *bio)
{
	semGive(bio->bio_caller1);
}

STATUS
readblk(device_t dev, int blkno)
{
	struct bio bio;
	unsigned blksize;
	STATUS status;

	printf("readblk(blkno=%d)\n", blkno);
	status = xbdBlockSize(dev, &blksize);
	if (status != OK) {
		printf("failed to get block size\n");
		goto error;
	}

	memset(&bio, 0, sizeof(bio));
	bio.bio_data = bio_cacheable_alloc(dev, 1);
	bio.bio_caller1 = semBCreate(SEM_Q_PRIORITY, SEM_EMPTY);
	if (!bio.bio_data || bio.bio_caller1 == SEM_ID_NULL)
		goto error;

	bio.bio_dev = dev;
	bio.bio_blkno = blkno;
	bio.bio_bcount = blksize;
	bio.bio_error = OK;
	bio.bio_flags = BIO_READ;
	bio.bio_done = ffsxbddone;
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

	bio_cacheable_free(bio.bio_data);
	semDelete(bio.bio_caller1);

	return OK;
}

STATUS
ffsdiskprobe(device_t dev)
{
	printf("%s:%d\n", __func__, __LINE__);
	readblk(dev, 0);
	return ERROR;
}

STATUS
ffsmonitordevcreate(device_t dev, char *devname)
{
	printf("%s:%d: %p %s\n", __func__, __LINE__, dev, devname);
	return OK;
}

void
ffsinit(void)
{
	printf("%s:%d\n", __func__, __LINE__);

	// when a device is mounted, the file system monitor will find the first handler matching the device
	// this probing only occurs on a create, if we use eject/remount operations the probe will not be called
	fsmProbeInstall(ffsdiskprobe, ffsmonitordevcreate);
}

STATUS
ffsblks(char *name, int blkstart, int blkend)
{
	device_t dev;
	int blkno;

	dev = fsmDevGetByName(name);
	if (!dev) {
		printf("failed to find device\n");
		return ERROR;
	}

	for (blkno = blkstart; blkno <= blkend; blkno++)
		readblk(dev, blkno);

	return OK;
}
