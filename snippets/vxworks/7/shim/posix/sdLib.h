#ifndef _SDLIB_H_
#define _SDLIB_H_

#include "types/vxTypes.h"
#include "objLib.h"
#include "errnoLib.h"

typedef void *SD_ID;

#define SD_ID_NULL NULL

#define SD_ATTR_RW 0x5b
#define SD_CACHE_OFF 0x80

SD_ID sdOpen(char *name, int options, int mode, size_t size, off_t physAddress, MMU_ATTR attr, void **pVirtAddress);
STATUS sdDelete(SD_ID sdId, int options);

#endif
