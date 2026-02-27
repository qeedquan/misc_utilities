#ifndef _SYSLIB_H_
#define _SYSLIB_H_

#include "types/vxTypes.h"
#include "sysctlCommon.h"

// These values are saved in a register that is read on startup to determine how to boot
enum {
	BOOT_NORMAL = 0x0,
	BOOT_NO_AUTOBOOT = 0x1,
	BOOT_CLEAR = 0x2,
	BOOT_QUICK_AUTOBOOT = 0x4,
};

typedef enum {
	MEM_DESC_RAM = 1,
	MEM_DESC_USER_RESERVED_RAM = 2,
	MEM_DESC_PM_RAM = 3,
	MEM_DESC_DMA32_RAM = 4,
	MEM_DESC_ROM = 5,
	MEM_DESC_KV_RAM = 6,
	MEM_DESC_RTP_RAM = 7,
	MEM_DESC_SHARED_RAM = 8,
	MEM_DESC_RGN_RAM_LAST,
} MEM_DESC_TYPE;

char *sysBspRev(void);
char *sysModel(void);

_Vx_freq_t sysClkRateGet(void);
_Vx_freq_t sysAuxClkRateGet(void);

int syscallGroupNumRtnGet(int syscallGroup, int *pNRtn);
int syscallNumArgsGet(int syscallNum, int *pNargs);

int sysProcNumGet(void);

int syscallInfo(int syscallNum, char *buffer, size_t *bufSize, int type);

STATUS sysNvRamGet(char *string, int strLen, int offset);
STATUS sysNvRamSet(char *string, int strLen, int offset);

PHYS_ADDR sysPhysMemTop();

enum {
	SCG_USER1,
};

#define SYSCALL_NUMBER(x, y) ((x) | (y))

#endif
