#ifndef _RANDOM_NUM_GEN_H_
#define _RANDOM_NUM_GEN_H_

typedef enum {
	RANDOM_NUM_GEN_NO_ENTROPY,
	RANDOM_NUM_GEN_NOT_ENOUGH_ENTROPY,
	RANDOM_NUM_GEN_ENOUGH_ENTROPY,
	RANDOM_NUM_GEN_MAX_ENTROPY,
	RANDOM_NUM_GEN_ERROR
} RANDOM_NUM_GEN_STATUS;

STATUS randBytes(unsigned char *randBuf, int numOfBytes);
STATUS randABytes(unsigned char *randBuf, int numOfBytes);
STATUS randUBytes(unsigned char *randBuf, int numOfBytes);

RANDOM_NUM_GEN_STATUS randStatus(void);
int randSecure(void);

#endif
