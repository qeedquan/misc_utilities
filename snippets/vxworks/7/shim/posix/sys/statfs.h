#ifndef _STATFS_H_
#define _STATFS_H_

#include "types/vxTypes.h"

enum {
	VD_SET_MAG = 0x06200556, // CDROMFS
	VD_FS_MAGIC = 0x12345432, // VDFS
	RAW_FS_MAGIC = 0x12344321, // RAWFS
	DOS_FS_MAGIC = 0xdfac9723, // DOSFS
	ROMFS_MAGIC = 0xDEADF00D, // ROMFS
	VRFS_MAGIC = 0x53465256, // VRFS
	NFSV2_MAGIC = 0x3253464E, // NFSv2
	NFSV3_MAGIC = 0x3353464E, // NFSv3
	HRFS_MAGIC = 0x48524653, // HRFS
	OVLFS_MAGIC = 0x4f564653, // OVLFS
};

#define fsid_t vxfsid_t
#define statfs vxstatfs
#define statfs64 vxstatfs64

typedef struct {
	long val[2];
} vxfsid_t;

struct vxstatfs {
	long f_type;
	long f_bsize;
	long f_blocks;
	long f_bfree;
	long f_bavail;
	long f_files;
	long f_ffree;
	fsid_t f_fsid;
	long f_spare[7];
};

struct vxstatfs64 {
	long f_type;
	long f_bsize;
	INT64 f_blocks;
	INT64 f_bfree;
	INT64 f_bavail;
	INT64 f_files;
	INT64 f_ffree;
	fsid_t f_fsid;
	long f_spare[7];
};

int vxstatfs(const char *path, struct vxstatfs *buf);
int vxstatfs64(const char *path, struct vxstatfs64 *buf);

#endif
