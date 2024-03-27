#include "vxWorks.h"
#include "hwif/vxBus.h"

char *
vxbDevNameGet(VXB_DEV_ID dev)
{
	return dev->name;
}

VXB_DEV_ID
vxbDevParent(VXB_DEV_ID dev)
{
	return dev->parent;
}

VXB_BUSTYPE_ID
vxbDevClassGet(VXB_DEV_ID dev)
{
	return "";
}

void *
vxbDevIvarsGet(VXB_DEV_ID dev)
{
	return dev->ivar;
}

void
vxbDevIvarsSet(VXB_DEV_ID dev, void *ivar)
{
	dev->ivar = ivar;
}

void
vxbAcquireByMatch(BOOL (*match)(VXB_DEV_ID, void *), void *ctx)
{
}
