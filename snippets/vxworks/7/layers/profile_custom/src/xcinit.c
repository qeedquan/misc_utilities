#include <vxWorks.h>
#include <sysLib.h>
#include <bootApp.h>
#include "xcinit.h"

void
xcinit(void)
{
	int i;

	for (i = 0; i < 10; i++)
		bootAppPrintf("Hello from Custom Layer\n", 0, 0, 0, 0, 0, 0);
}
