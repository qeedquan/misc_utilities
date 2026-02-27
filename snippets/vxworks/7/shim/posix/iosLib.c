#include "vxWorks.h"

STATUS
iosDevAdd(DEV_HDR *pDevHdr, const char *name, int drvnum)
{
	return OK;
}

int
iosDrvInstall(
    DRV_CREATE_PTR pCreate,
    DRV_REMOVE_PTR pRemove,
    DRV_OPEN_PTR pOpen,
    DRV_CLOSE_PTR pClose,
    DRV_READ_PTR pRead,
    DRV_WRITE_PTR pWrite,
    DRV_IOCTL_PTR pIoctl)
{
	return 0;
}
