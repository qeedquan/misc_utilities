#include <vxWorks.h>
#include <stdio.h>
#include <end.h>
#include <muxLib.h>

void
endshow(void)
{
	DEV_OBJ *dev;
	END_OBJ *ends[32];
	int numends;
	int i;

	numends = muxEndListGet(ends, NELEMENTS(ends));
	for (i = 0; i < numends; i++) {
		dev = &ends[i]->devObject;
		printf("%s:%d | %s\n", dev->name, dev->unit, dev->description);
	}
}
