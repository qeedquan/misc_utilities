#ifndef _MEMLIB_H_
#define _MEMLIB_H_

#include "types/vxTypes.h"
#include "memPartLib.h"
#include "errnoLib.h"

#define S_memLib_NOT_ENOUGH_MEMORY ENOMEM
#define S_memLib_INVALID_NBYTES (M_memLib | 2)
#define S_memLib_BLOCK_ERROR (M_memLib | 3)
#define S_memLib_NO_PARTITION_DESTROY (M_memLib | 4)
#define S_memLib_PAGE_SIZE_UNAVAILABLE (M_memLib | 5)
#define S_memLib_WRONG_PART_ID (M_memLib | 6)
#define S_memLib_INVALID_ADDRESS (M_memLib | 7)
#define S_memLib_INVALID_ALIGNMENT (M_memLib | 8)
#define S_memLib_FUNC_NOT_AVAILABLE (M_memLib | 9)
#define S_memLib_MEM_ALLOC_ERROR_NO_ALLOC_SET (M_memLib | 10)
#define S_memLib_RTP_MEM_ALLOC_ERROR_NO_ALLOC_SET (M_memLib | 11)
#define S_memLib_RTP_MEM_FREE_ERROR_NO_ALLOC_SET (M_memLib | 12)

STATUS memInfoGet(MEM_PART_STATS *pPartStats);
size_t memFindMax(void);

#endif
