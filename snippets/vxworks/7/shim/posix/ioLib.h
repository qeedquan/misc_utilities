#ifndef _IOLIB_H_
#define _IOLIB_H_

#include <errno.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/statfs.h>
#include "rtpLibCommon.h"

size_t rtpIoTableSizeGet(RTP_ID rtpId);

enum {
	S_ioLib_NO_DRIVER = ENXIO,
	S_ioLib_UNKNOWN_REQUEST = ENOTSUP,
	S_ioLib_DEVICE_ERROR = EIO,
	S_ioLib_DEVICE_TIMEOUT = EIO,
	S_ioLib_WRITE_PROTECTED = EACCES,
	S_ioLib_DISK_NOT_PRESENT = ENXIO,
	S_ioLib_NO_FILENAME = ENOENT,
	S_ioLib_CANCELLED = ECANCELED,
	S_ioLib_NO_DEVICE_NAME_IN_PATH = ENODEV,
	S_ioLib_NAME_TOO_LONG = ENAMETOOLONG,
	S_ioLib_UNFORMATED = EIO,
	S_ioLib_CANT_OVERWRITE_DIR = EISDIR,
};

enum {
	FIOFLUSH = 2,
	FIOOPTIONS = 3,
	FIOBAUDRATE = 4,
	FIODISKFORMAT = 5,
	FIODISKINIT = 6,
	FIOSEEK = 7,
	FIOWHERE = 8,
	FIODIRENTRY = 9,
	FIORENAME = 10,
	FIOREADYCHANGE = 11,
	FIONWRITE = 12,
	FIODISKCHANGE = 13,
	FIOCANCEL = 14,
	FIOSQUEEZE = 15,
	FIONMSGS = 17,
	FIOGETNAME = 18,

	FIOUNMOUNT = 39,

	FIOCOMMITPOLICYGETFS = 66,
	FIOCOMMITPOLICYSETFS = 67,
	FIOCOMMITPERIODGETFS = 68,

	FIOCOMMITPERIODSETFS = 69,

	FIODISCARDGET = 70,
	FIODISCARD = 71,
};

typedef struct dev_hdr {
	const char *name;
} DEV_HDR;

typedef struct {
	long long startSector;
	unsigned int nSectors;
} SECTOR_RANGE;

typedef void *(*DRV_CREATE_PTR)(DEV_HDR *devhdr, const char *filename, int flags);
typedef void *(*DRV_OPEN_PTR)(DEV_HDR *devhdr, const char *filename, int flags, int mode);
typedef void (*DRV_REMOVE_PTR)(DEV_HDR *devhdr, const char *filename);
typedef int (*DRV_CLOSE_PTR)(void *fd);
typedef ssize_t (*DRV_READ_PTR)(void *fd, char *buf, size_t len);
typedef ssize_t (*DRV_WRITE_PTR)(void *fd, const char *buf, size_t len);
typedef int (*DRV_IOCTL_PTR)(void *fd, int func, _Vx_ioctl_arg_t arg);

#endif
