#ifndef _FLASH_H_
#define _FLASH_H_

#include "types/vxTypes.h"

typedef UINT32 FLASH_BLK_ADDR_T;
typedef UINT32 FLASH_BLK_SIZE_T;

typedef UINT16 FLASH_PAGE_ADDR_T;
typedef UINT16 FLASH_PAGE_SIZE_T;
typedef UINT16 FLASH_OOB_SIZE_T;
typedef UINT32 FLASH_COL_ADDR_T;

typedef UINT64 FLASH_FLAT_ADDR_T;

typedef enum flash_type_t {
	FLASH_TYPE_NOR = 1,
	FLASH_TYPE_NAND,
} FLASH_TYPE;

typedef enum flash_cell_t {
	FLASH_CELL_NONE = 0,
	FLASH_CELL_SLC,
	FLASH_CELL_MLC,
} FLASH_CELL;

typedef enum flash_usage_type_t {
	FLASH_USAGE_FTL = 0,
	FLASH_USAGE_NVRAM,
} FLASH_USAGE_TYPE;

typedef struct flash_norm_addr_t {
	FLASH_BLK_ADDR_T blkAddr;
	FLASH_PAGE_ADDR_T pageAddr;
	FLASH_COL_ADDR_T colAddr;
} FLASH_NORM_ADDR_T, *pFLASH_NORM_ADDR_T;

typedef struct flash_norm_addr_al_t {
	FLASH_BLK_ADDR_T blkAddr;
	FLASH_PAGE_ADDR_T pageAddr;
} FLASH_NORM_ADDR_AL_T, *pFLASH_NORM_ADDR_AL_T;

typedef struct flash_sgbuf_t flash_sgbuf_t;
typedef flash_sgbuf_t FLASH_SGBUF_T;
typedef FLASH_SGBUF_T *pFLASH_SGBUF_T;

struct flash_sgbuf_t {
	FLASH_NORM_ADDR_T addr;
	FLASH_PAGE_SIZE_T uLen;
	UINT8 *pBuf;
	FLASH_SGBUF_T *pNext;
	void *pPriv;
	void *pOp;
};

#endif
