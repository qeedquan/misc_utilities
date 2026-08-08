#ifndef _VXB_LIB_H_
#define _VXB_LIB_H_

#include "hwif/vxbus/vxbDevDef.h"

#define MAX_DRV_NAME_LEN 31
#define VXB_MAXBARS 10

#define VXB_PROBE_HARD_MATCH 0
#define VXB_PROBE_HARD_NOMATCH -1
#define VXB_PROBE_HARD_ERROR -2

typedef UINT64 VXB_KEY;	

typedef struct vxbDrvDeferEntry {
	SL_NODE vxbNode;
	char vxbName[MAX_DRV_NAME_LEN];
} VXB_DRV_DEFER_ENTRY;

typedef struct vxbDev *VXB_DEV_ID;

#endif
