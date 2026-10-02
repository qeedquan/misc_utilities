#ifndef _VXB_BUSTYPE_H_
#define _VXB_BUSTYPE_H_

#include "sllLib.h"

typedef char *VXB_BUSTYPE_ID;

typedef struct vxbBusType {
	SL_NODE vxbNode;
	char *vxbBusName;
	char *vxbBusDesc;
} VXB_BUSTYPE;

#define VXB_BUSID_MAX_LEN 256

#define VXB_BUSID_ROOT "root"
#define VXB_BUSID_NONE NULL

#endif
