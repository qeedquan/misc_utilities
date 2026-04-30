/*

Test block device functionality using the XBD subsystem

*/

#include <vxWorks.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <semLib.h>
#include <xbdRamDisk.h>
#include <drv/erf/erfLib.h>
#include <fsMonitor.h>
#include <xbdPartition.h>
#include <fsEventUtilLib.h>
#include <taskLib.h>
#include <private/iosLibP.h>

typedef struct {
	// must be first field
	XBD xbd;
	struct xbd_funcs funcs;

	BOOL instantiated;
	UINT16 insert_event_type;

	char *data;
	unsigned len;

	unsigned blksz;
	unsigned nblks;
} XD;

LOCAL int
xdioctl(struct xbd *xbd, int cmd, void *arg)
{
	STATUS status;
	XBD_INFO *info;
	XBD_GEOMETRY *geo;
	XD *xd;
	void *erfparam;

	xd = (XD *)xbd;
	erfparam = (void *)(uintptr_t)xd->xbd.xbd_dev.dv_dev;

	printf("%s(xbd=%p, cmd=%d, arg=%p)\n", __func__, xbd, cmd, arg);
	switch (cmd) {
	case XBD_GETINFO:
		if (arg == NULL)
			return ENXIO;

		info = arg;
		geo = &info->xbdGeometry;
		memset(info, 0, sizeof(*info));
		info->mediaType = XBD_MEDIA_RAM;
		geo->secs_per_track = 16;
		geo->heads = 1;
		geo->cylinders = xd->nblks / info->xbdGeometry.secs_per_track;
		geo->total_blocks = xd->nblks;
		geo->blocksize = xd->blksz;
		break;

	case XBD_GETGEOMETRY:
		if (arg == NULL)
			return EINVAL;
		geo = arg;
		geo->secs_per_track = 16;
		geo->heads = 1;
		geo->cylinders = xd->nblks / geo->secs_per_track;
		geo->total_blocks = xd->nblks;
		geo->blocksize = xd->blksz;
		break;

	case XBD_SOFT_EJECT:
	case XBD_HARD_EJECT:
		erfEventRaise(xbdEventCategory, xbdEventRemove, ERF_ASYNC_PROC, erfparam, NULL);
		erfEventRaise(xbdEventCategory, (cmd == XBD_HARD_EJECT) ? xd->insert_event_type : xbdEventSoftInsert, ERF_ASYNC_PROC, erfparam, NULL);
		break;

	case XBD_GETBASENAME:
		status = devName(xbd->xbd_dev.dv_dev, arg);
		if (status == ERROR)
			return errno;
		break;

	case XBD_SYNC:
		break;

	case XBD_STACK_COMPLETE:
		if (!xd->instantiated) {
			erfEventRaise(xbdEventCategory, xbdEventInstantiated, ERF_ASYNC_PROC, xd->xbd.xbd_dev.dv_xname, NULL);
			xd->instantiated = TRUE;
		}
		break;

	default:
		return ENOTSUP;
	}
	return 0;
}

// handle read/write commands
LOCAL int
xdstrategy(struct xbd *xbd, struct bio *bio)
{
	XD *xd;
	int status;
	unsigned i;
	char *ptr;

	xd = (XD *)xbd;
	printf("%s(xbd=%p, bio=%p)\n", __func__, xd, bio);
	for (; bio != NULL; bio = bio->bio_chain) {
		status = 0;

		// this is a readonly block device
		if (bio->bio_flags & BIO_WRITE) {
			status = EROFS;
		} else if (bio->bio_flags & BIO_READ) {
			ptr = bio->bio_data;
			for (i = 0; i < bio->bio_bcount; i++) {
				ptr[i] = xd->data[i % xd->len];
			}
		}

		bio_done(bio, status);
	}
	return 0;
}

// write data with size starting at block number
LOCAL int
xddump(struct xbd *xbd, sector_t blkno, void *data, size_t size)
{
	struct bio bio;

	printf("%s(xbd=%p, blkno=%d, data=%p, size=%zu)\n", __func__, xbd, blkno, data, size);
	memset(&bio, 0, sizeof(bio));
	bio.bio_dev = xbd->xbd_dev.dv_dev;
	bio.bio_blkno = blkno;
	bio.bio_bcount = size;
	bio.bio_data = data;
	bio.bio_flags = BIO_WRITE;

	xdstrategy(xbd, &bio);
	return bio.bio_error;
}

device_t
xdcreate(char *name, char *data)
{
	XD *xd;
	device_t dev;

	xd = NULL;
	if (name == NULL || name[0] == EOS || strlen(name) >= sizeof(devname_t)) {
		errno = EINVAL;
		goto error;
	}

	if (iosDevFindExact(name) != NULL || devGetByName(name) != NULLDEV) {
		errno = EEXIST;
		goto error;
	}

	xd = calloc(1, sizeof(*xd));
	if (xd)
		xd->data = strdup(data);
	if (!xd || !xd->data) {
		errno = ENOMEM;
		goto error;
	}

	xd->funcs.xf_strategy = xdstrategy;
	xd->funcs.xf_ioctl = xdioctl;
	xd->funcs.xf_dump = xddump;

	xd->len = strlen(xd->data);
	xd->blksz = 512;
	xd->nblks = xd->len / xd->blksz;
	if (xd->len % xd->blksz)
		xd->nblks++;

	xbdAttach((struct xbd *)xd, &xd->funcs, name, xd->blksz, xd->nblks, &dev);

	xd->insert_event_type = xbdEventSecondaryInsert;
	erfEventRaise(xbdEventCategory, xd->insert_event_type, ERF_SYNC_PROC, (void *)(uintptr_t)dev, NULL);

	if (0) {
	error:
		if (xd)
			free(xd->data);
		free(xd);
		dev = NULLDEV;
	}

	return dev;
}

void
xdtestio(const char *name)
{
	int fd;
	char buf[16];
	ssize_t len;

	fd = open(name, O_RDWR);
	if (fd < 0) {
		printf("Failed to XBD device: %s: %s\n", name, strerror(errno));
		return;
	}

	memset(buf, 0xff, sizeof(buf));
	if ((len = write(fd, buf, sizeof(buf))) < 0)
		printf("Failed to write: %s\n", strerror(errno));

	len = read(fd, buf, sizeof(buf));
	if (len <= 0)
		printf("Failed to read: %s\n", strerror(errno));
	else {
		buf[len - 1] = '\0';
		printf("Read: %s\n", buf);
	}

	close(fd);
}

void
xdtest(void)
{
	device_t dev;
	devname_t basename;

	dev = xdcreate("/xd", "hello");
	if (!dev) {
		printf("Failed to create XBD device\n");
		return;
	}

	if (xbdIoctl(dev, XBD_GETBASENAME, basename) != OK) {
		printf("Failed to get XBD base name\n");
		return;
	}

	printf("XBD device name %s\n", basename);

	xdtestio("/xd");
}
