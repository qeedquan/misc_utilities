#include "vxWorks.h"
#include "hwif/vxBus.h"

char *
vxbDevNameGet(VXB_DEV_ID dev)
{
	return NULL;
}

VXB_DEV_ID
vxbDevParent(VXB_DEV_ID dev)
{
	return NULL;
}

VXB_BUSTYPE_ID
vxbDevClassGet(VXB_DEV_ID dev)
{
	return "";
}

void *
vxbDevIvarsGet(VXB_DEV_ID dev)
{
	return NULL;
}

void
vxbDevIvarsSet(VXB_DEV_ID dev, void *ivar)
{
}

void
vxbAcquireByMatch(BOOL (*match)(VXB_DEV_ID, void *), void *ctx)
{
}
