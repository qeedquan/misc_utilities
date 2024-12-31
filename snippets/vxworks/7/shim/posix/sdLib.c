#include "vxWorks.h"

SD_ID
sdOpen(char *name, int options, int mode, size_t size, off_t physAddress, MMU_ATTR attr, void **pVirtAddress)
{
	return SD_ID_NULL;
}

STATUS
sdDelete(SD_ID sdId, int options)
{
	return OK;
}
