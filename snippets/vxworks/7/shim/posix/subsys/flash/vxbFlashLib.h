#ifndef _VXBFLASHLIB_H_
#define _VXBFLASHLIB_H_

#include "hwif/vxbus/vxbLib.h"

typedef struct vxbFlashCtrl {
	SL_NODE flashNode;
	VXB_DEV_ID pDev;
	char devName[32];
	UINT32 unitId;
	void *flashChip;
	ULONG private;

	STATUS (*flashShow)(VXB_DEV_ID, UINT32);
} VXB_FLASHCTRL;

STATUS vxbFlashChipAdd(VXB_FLASHCTRL *chip);
void *vxbFlashChipFind(char *devname, UINT32 unit);
ULONG vxbFlashPrivGet(char *devname, UINT32 unit);

#endif
