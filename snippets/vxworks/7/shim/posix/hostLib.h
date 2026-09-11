#ifndef _HOSTLIB_H_
#define _HOSTLIB_H_

#include "types/vxTypes.h"
#include "errnoLib.h"

enum {
	MAXHOSTNAMELEN = 64,
};

void hostTblInit(void);

STATUS hostAdd(char *name, char *addr);
STATUS hostDelete(char *name, char *addr);
int hostGetByName(char *name);
STATUS hostGetByAddr(int addr, char *name);

#endif
