#ifndef _MEMPARTLIB_H_
#define _MEMPARTLIB_H_

#include "types/vxTypes.h"
#include "memLib.h"

typedef uintptr_t PART_ID;

struct wind_part_stats {
	unsigned long numBytesFree;
	unsigned long numBlocksFree;
	unsigned long numBytesAlloc;
	unsigned long numBlocksAlloc;
	unsigned long maxBytesAlloc;
	unsigned long maxBlockSizeFree;
};

typedef struct wind_part_stats MEM_PART_STATS;

#endif
