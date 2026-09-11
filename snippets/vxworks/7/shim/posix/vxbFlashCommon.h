#ifndef _VXB_FLASH_COMMON_H_
#define _VXB_FLASH_COMMON_H_

#include "flash.h"

#define FLASH_CHIP_NAME_LEN 64

typedef struct flash_sgrw_param {
	FLASH_SGBUF_T *pBuf;
	FLASH_SIZE_T uBufCnt;
	FLASH_SGBUF_T *pErrBuf;
	UINT32 uFlag;
	void *pOp;
} FLASH_SGRW_PARAM, *pFLASH_SGRW_PARAM;

typedef struct flash_bbchk_param {
	FLASH_BLK_ADDR_T uBlkAddr;
	BOOL bBadBlk;
	void *pOp;
} FLASH_BBCHK_PARAM, *pFLASH_BBCHK_PARAM;

typedef struct flash_seqrw_param {
	FLASH_ADDR_T uAddr;
	UINT32 uLen;
	UINT8 *pBuf;
	FLASH_SIZE_T uFinLen;
	FLASH_ADDR_T uErrAddr;
	UINT32 uFlag;
	void *pOp;
} FLASH_SEQRW_PARAM, *pFLASH_SEQRW_PARAM;

typedef struct flash_blkera_param {
	FLASH_BLK_ADDR_T uBlkAddr;
	FLASH_BLK_ADDR_T uBlkCnt;
	FLASH_BLK_ADDR_T uFinBlkCnt;
	void *pOp;
} FLASH_BLKERA_PARAM, *pFLASH_BLKERA_PARAM;

typedef struct flash_mv_param {
	FLASH_ADDR_T uSrcAddr;
	FLASH_ADDR_T uDstAddr;
	FLASH_SIZE_T uPageCnt;
	FLASH_SIZE_T uFinPageCnt;
	void *pOp;
} FLASH_MV_PARAM, *pFLASH_MV_PARAM;

typedef struct flash_blklock_param {
	FLASH_BLK_ADDR_T uBlkAddr;
	FLASH_BLK_ADDR_T uBlkCnt;
	BOOL lock;
	FLASH_BLK_ADDR_T uFinBlkCnt;
	void *pOp;
} FLASH_LOCK_PARAM, *pFLASH_LOCK_PARAM;

typedef struct flash_chip flash_chip;
typedef struct flash_chip FLASH_CHIP, *pFLASH_CHIP, *FLASH_CHIP_ID;

typedef struct flash_ops {
	int (*rst)(FLASH_CHIP_ID);

	int (*seqRead)(FLASH_CHIP_ID, pFLASH_SEQRW_PARAM);
	int (*seqWrite)(FLASH_CHIP_ID, pFLASH_SEQRW_PARAM);

	int (*sgRead)(FLASH_CHIP_ID, pFLASH_SGRW_PARAM);
	int (*sgWrite)(FLASH_CHIP_ID, pFLASH_SGRW_PARAM);

	int (*eBlkErase)(FLASH_CHIP_ID, pFLASH_BLKERA_PARAM);
	int (*move)(FLASH_CHIP_ID, pFLASH_MV_PARAM);

	int (*eBlkLock)(FLASH_CHIP_ID, pFLASH_LOCK_PARAM);
	int (*blkLockChk)(FLASH_CHIP_ID, pFLASH_LOCK_PARAM);

	int (*bbChk)(FLASH_CHIP_ID, pFLASH_BBCHK_PARAM);

	int (*ena)(FLASH_CHIP_ID, BOOL, void *);

	int (*read)(FLASH_CHIP_ID, FLASH_ADDR_T, UINT32, FLASH_SIZE_T, UINT8 **, void *);
	int (*write)(FLASH_CHIP_ID, FLASH_ADDR_T, UINT32, FLASH_SIZE_T, UINT8 **, void *);

	int (*oobRead)(FLASH_CHIP_ID, FLASH_ADDR_T, UINT32, UINT8 **, FLASH_SIZE_T);
	int (*oobWrite)(FLASH_CHIP_ID, FLASH_ADDR_T, UINT32, UINT8 **, FLASH_SIZE_T);

	int (*blkErase)(FLASH_CHIP_ID, FLASH_ADDR_T, UINT32);

	int (*blkLock)(FLASH_CHIP_ID, FLASH_ADDR_T, BOOL);
	int (*blkLockQry)(FLASH_CHIP_ID, FLASH_ADDR_T, BOOL);

} FLASH_OPS;

typedef FLASH_OPS *pFLASH_OPS;

typedef enum flash_chip_type {
	FLASH_CHIP_TYPE_NOR = 1,
	FLASH_CHIP_TYPE_NAND = 2,
	FLASH_CHIP_TYPE_SPI = 3,
	FLASH_CHIP_TYPE_HYPER = 4,
} FLASH_CHIP_TYPE;

struct flash_chip {
	void *pDev;

	UINT8 chipName[FLASH_CHIP_NAME_LEN];
	FLASH_SIZE_T uChipSize;
	FLASH_BLK_SIZE_T uEraseSize;
	FLASH_PAGE_SIZE_T uPageSize;

	FLASH_OOB_SIZE_T uOobAvlSize;

	FLASH_CHIP_TYPE uFlashType;
	UINT32 uCapability;
	FLASH_OPS flashOPs;

	void *pFlashInfo;
	void *pPriv;
};

typedef enum flash_err_type {
	FLASH_INV_PARAM_ERR = 1,
	FLASH_OP_FAIL_ERR,
	FLASH_CORRECT_ECC_ERR,
	FLASH_UNCORRECT_ECC_ERR,
	FLASH_FAKE_ECC_ERR,
	FLASH_OTHER_ERR,
} FLASH_ERR_TYPE;

enum {
	FLASH_CHIP_CAP_NSEQ_PROG = 0x00040000,
};

enum {
	FLASH_CHIP_CAP_RD = 0x01000000,
	FLASH_CHIP_CAP_WR = 0x02000000,
	FLASH_CHIP_CAP_OOBRD = 0x04000000,
	FLASH_CHIP_CAP_OOBWR = 0x08000000,
	FLASH_CHIP_CAP_BLKERA = 0x10000000,
	FLASH_CHIP_CAP_BLKLOCK = 0x20000000,
	FLASH_CHIP_CAP_BLKLOCKQRY = 0x40000000,
};

#endif
