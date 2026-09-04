#include "vxWorks.h"
#include "hwif/vxBus.h"
#include "hwif/util/vxbAccess.h"

STATUS
vxbRegMap(VXB_RESOURCE *res)
{
	return OK;
}

STATUS
vxbRegUnmap(VXB_RESOURCE *res)
{
	return OK;
}

STATUS
vxbRegUnmapAll(VXB_DEV_ID dev)
{
	return OK;
}

UINT8
vxbRead8(void *handle, UINT8 *offset)
{
	return 0;
}

UINT16
vxbRead16(void *handle, UINT16 *offset)
{
	return 0;
}

UINT32
vxbRead32(void *handle, UINT32 *offset)
{
	return 0;
}

void
vxbWrite8(void *handle, UINT8 *offset, UINT8 val)
{
}

void
vxbWrite16(void *handle, UINT16 *offset, UINT16 val)
{
}

void
vxbWrite32(void *handle, UINT32 *offset, UINT32 val)
{
}
