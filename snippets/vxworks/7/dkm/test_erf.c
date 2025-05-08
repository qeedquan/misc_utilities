/*

Test Event Reporting Functions (erf)
ERF is a way to raise events and let a event task process them in the background

*/

#include <vxWorks.h>
#include <stdio.h>
#include <unistd.h>
#include <drv/erf/erfLib.h>
#include <drv/erf/erfShow.h>

void
erfinfo(void)
{
	printf("Max Categories: %d\n", erfMaxCategoriesGet());
	printf("Max Types:      %d\n", erfMaxTypesGet());
	printf("Queue Size:     %d\n", erfDefaultQueueSizeGet());
	printf("Categories:     %d\n", erfCategoriesAvailable());
	erfShow();
}

void
erfhandler(UINT16 maxusercat, UINT16 maxusertype, void *eventdata, void *userdata)
{
	printf("GOT AN ERF: %d %d %p %p\n", maxusercat, maxusertype, eventdata, userdata);
}

void
erftest(void)
{
	UINT16 evcat;
	UINT16 cat[2];
	size_t i;

	if (erfCategoryAllocate(&evcat) != OK) {
		printf("Failed to allocate event category");
		return;
	}

	for (i = 0; i < NELEMENTS(cat); i++) {
		if (erfTypeAllocate(evcat, &cat[i]) != OK) {
			printf("Failed to allocate category %d\n", i);
		}
		erfHandlerRegister(evcat, cat[i], erfhandler, (void *)i, 0);
	}
	sleep(1);

	for (i = 0; i < NELEMENTS(cat); i++) {
		erfEventRaise(evcat, cat[i], ERF_ASYNC_PROC, NULL, NULL);
	}
}
