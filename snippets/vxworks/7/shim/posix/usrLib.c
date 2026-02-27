#include "vxWorks.h"

void
version(void)
{
}

STATUS
memoryDump(void *adrs, int units, int width, const char *errorStr)
{
	return OK;
}

void
d(void *adrs, int nunits, int width)
{
	memoryDump(adrs, nunits, width, "d()");
}
