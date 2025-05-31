#ifndef _VXB_DEV_DEF_H_
#define _VXB_DEV_DEF_H_

#include "types/vxTypes.h"
#include "sllLib.h"

typedef struct vxbDev {
	SL_NODE vxbNode;
	SL_NODE vxbAttachNode;

	void *pVxbSoftc;
	void *pVxbDrvData;

	UINT32 vxbUnit;

	char *pName;

	UINT32 level;
} VXB_DEV;

#endif
