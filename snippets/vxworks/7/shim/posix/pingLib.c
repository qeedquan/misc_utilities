#include "vxWorks.h"
#include "taskLib.h"
#include "pingLib.h"

STATUS
pingLibInit(void)
{
	return OK;
}

STATUS
ping(char *host, int numPackets, ulong_t options)
{
	return OK;
}
