#ifndef _XBD_H_
#define _XBD_H_

#include "types/vxTypes.h"
#include "drv/manager/device.h"
#include "drv/xbd/bio.h"

struct xbd;
struct xbd_requests;

typedef struct xbd_requests {
	void *drvSpecific;
	struct xbd *pXbd;
	struct xbd_request *pNext;
} XBD_REQUEST, *pXBD_REQUEST;

typedef enum xbd_level {
	XBD_TOP = 0,
	XBD_PART = 1,
	XBD_BASE = 2,
} XBD_LEVEL;

struct xbd_funcs {
	int (*xf_ioctl)(struct xbd *dev, int cmd, void *arg);
	int (*xf_strategy)(struct xbd *dev, struct bio *bio);
	int (*xf_dump)(struct xbd *dev, sector_t pos, void *data, size_t size);
	int (*xf_request)(struct xbd *dev, struct xbd_request *req);
};

enum {
	XBD_EXT_MAGIC = 0xABCDDCBA,
};

enum {
	BIO_SCHED_FIFO = 0,
	BIO_SCHED_ELEVATOR = 1,
	BIO_SCHED_DIRECT = 2,
	BIO_SCHED_SSD = 3,
	BIO_SCHED_DEADLINE = 4,
	BIO_SCHED_NOOP = 5,
};

enum {
	XBD_GETGEOMETRY = 0xbd000001,
	XBD_SYNC = 0xbd000002,
	XBD_SOFT_EJECT = 0xbd000003,
	XBD_HARD_EJECT = 0xbd000004,
	XBD_GETBASENAME = 0xbd000005,
	XBD_TEST = 0xbd000006,
	XBD_STACK_COMPLETE = 0xbd000007,
	XBD_GETINFO = 0xbd000008,

	XBD_GET_BIOSCHED = 0xbd000009,
	XBD_SET_BIOSCHED = 0xbd00000a,
	XBD_GET_DEVTYPE = 0xbd00000b,

	XBD_SMART_ENABLE = 0xbd000011,
	XBD_SMART_DISABLE = 0xbd000012,
};

enum {
	XBD_MEDIA_LOOPFS = 0x4C50,
	XBD_MEDIA_RAM = 0x524D,
	XBD_MEDIA_SATA = 0x5354,
	XBD_MEDIA_USB = 0x5542,
	XBD_MEDIA_MMC = 0x4D43,
	XBD_MEDIA_SD = 0x5344,
	XBD_MEDIA_FTL = 0x464C,
	XBD_MEDIA_NVME = 0x4E4D,
	XBD_MEDIA_TFFS = 0x5446,
	XBD_MEDIA_SOFTXBD = 0x5358,
	XBD_MEDIA_VIRTIOXBD = 0x5654,
	XBD_MEDIA_UFSXBD = 0x5546,
};

typedef enum xbd_type {
	XBD_FDD = 1,
	XBD_HDD = 2,
} XBD_TYPE;

typedef struct xbd_geometry {
	unsigned heads;
	unsigned long long cylinders;
	sector_t total_blocks;
	unsigned secs_per_track;
	unsigned blocksize;
} XBD_GEOMETRY;

typedef struct xbd_info {
	XBD_GEOMETRY xbdGeometry;
	uint32_t properties;
	uint32_t mediaType;
	uint32_t reserved[3];
} XBD_INFO;

typedef struct xbd {
	struct xbd_funcs *xbd_funcs;
	unsigned xbd_blocksize;
	sector_t xbd_nblocks;

	atomic_t xbd_flags;
} XBD, *XBD_ID;

#endif
