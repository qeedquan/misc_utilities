#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <isrLib.h>
#include <private/isrLibP.h>
#include <private/classLibP.h>
#include <private/objLibP.h>

LOCAL BOOL
isrprint(OBJ_ID objid, void *arg)
{
	ISR_DESC desc;
	ISR_ID isrid;

	isrid = objid;
	if (isrInfoGet(isrid, &desc) != OK) {
		printf("Error: %p is not a valid ISR object.\n", isrid);
		return TRUE;
	}

	printf("%-12s %-20p %-4lx %-8u %-8u %-20p %-20lx %-8u\n",
	       desc.name, desc.isrId, desc.isrTag, desc.count, desc.cpuTime, desc.handlerRtn, desc.arg, desc.options);

	return TRUE;

	(void)arg;
}

void
isrinfo(void)
{
	printf("ISR Info\n");
	printf("%-12s %-20s %-4s %-8s %-8s %-20s %-20s %-8s\n",
	       "name", "id", "tag", "count", "cputime", "handler", "arg", "options");
	objEach(windIsrClass, isrprint, NULL, FALSE);
}
