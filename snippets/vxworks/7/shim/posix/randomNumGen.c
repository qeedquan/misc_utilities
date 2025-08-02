#include "vxWorks.h"
#include "randomNumGen.h"

STATUS
randBytes(unsigned char *randBuf, int numOfBytes)
{
	return OK;
}

STATUS
randABytes(unsigned char *randBuf, int numOfBytes)
{
	return OK;
}

STATUS
randUBytes(unsigned char *randBuf, int numOfBytes)
{
	return OK;
}

RANDOM_NUM_GEN_STATUS
randStatus(void)
{
	return RANDOM_NUM_GEN_MAX_ENTROPY;
}

int
randSecure(void)
{
	return 1;
}