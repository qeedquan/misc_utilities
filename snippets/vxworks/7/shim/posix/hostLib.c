#include "vxWorks.h"

typedef struct {
	char name[32];
	char addr[32];
} Host;

void
hostTblInit(void)
{
}

STATUS
hostAdd(char *name, char *addr)
{
	return OK;
}

STATUS
hostDelete(char *name, char *addr)
{
	return OK;
}

int
hostGetByName(char *name)
{
	return 0;
}

STATUS
hostGetByAddr(int addr, char *name)
{
	return OK;
}
