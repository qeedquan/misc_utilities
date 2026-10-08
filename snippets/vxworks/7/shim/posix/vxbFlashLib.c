#include "vxWorks.h"
#include "subsys/flash/vxbFlashLib.h"

static VXB_FLASHCTRL *flashctrls[128];
static size_t nflashctrl;

STATUS
vxbFlashChipAdd(VXB_FLASHCTRL *flashctrl)
{
	if (nflashctrl >= nelem(flashctrls))
		return ERROR;

	flashctrls[nflashctrl++] = flashctrl;
	return OK;
}

LOCAL VXB_FLASHCTRL *
findflash(char *devname, UINT32 unit)
{
	VXB_FLASHCTRL *flashctrl;
	size_t i;

	for (i = 0; i < nflashctrl; i++) {
		flashctrl = flashctrls[i];
		if (!strcmp(flashctrl->devName, devname) && flashctrl->unitId == unit)
			return flashctrl;
	}
	return NULL;
}

void *
vxbFlashChipFind(char *devname, UINT32 unit)
{
	VXB_FLASHCTRL *flashctrl;

	flashctrl = findflash(devname, unit);
	if (flashctrl)
		return flashctrl->flashChip;
	return NULL;
}

ULONG
vxbFlashPrivGet(char *devname, UINT32 unit)
{
	VXB_FLASHCTRL *flashctrl;

	flashctrl = findflash(devname, unit);
	if (flashctrl)
		return flashctrl->private;
	return 0;
}
