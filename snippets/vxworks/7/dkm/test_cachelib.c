#include <vxWorks.h>
#include <stdio.h>
#include <stdlib.h>
#include <cacheLib.h>
#include <aimCacheLib.h>

STATUS
test_cache(void)
{
	STATUS status;
	int *ptr;

	status = OK;
	ptr = malloc(sizeof(*ptr));
	if (!ptr)
		goto error;

	cacheInvalidate(INSTRUCTION_CACHE, 0, ENTIRE_CACHE);
	cacheInvalidate(DATA_CACHE, ptr, 4);
	cacheClear(DATA_CACHE, NULL, ENTIRE_CACHE);

	if (0) {
	error:
		status = ERROR;
	}

	free(ptr);

	return status;
}
