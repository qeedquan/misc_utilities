#include "vxWorks.h"

char *
sysBspRev(void)
{
	return "1.0";
}

char *
sysModel(void)
{
	return "1.0";
}

_Vx_freq_t
sysClkRateGet(void)
{
	return VX_SYSCLK_RATE;
}

_Vx_freq_t
sysAuxClkRateGet(void)
{
	return VX_SYSCLK_RATE;
}

int
syscallGroupNumRtnGet(int syscallGroup, int *pNRtn)
{
	return 0;
}

int
syscallNumArgsGet(int syscallNum, int *pNargs)
{
	return 0;
}

int
sysProcNumGet(void)
{
	return 0;
}

int
syscallInfo(int syscallNum, char *buffer, size_t *bufSize, int type)
{
	return 0;
}

char nvram[1 * 1024 * 1024];
int nvramsize = sizeof(nvram);

STATUS
sysNvRamGet(char *string, int strLen, int offset)
{
	if (strLen < 0 || offset < 0 || strLen >= nvramsize || offset >= nvramsize || strLen + offset >= nvramsize)
		return ERROR;
	memcpy(string, nvram + offset, strLen);
	return OK;
}

STATUS
sysNvRamSet(char *string, int strLen, int offset)
{
	if (strLen < 0 || offset < 0 || strLen >= nvramsize || offset >= nvramsize || strLen + offset >= nvramsize)
		return ERROR;
	memcpy(nvram + offset, string, strLen);
	return OK;
}

PHYS_ADDR
sysPhysMemTop(void)
{
	return 0;
}
