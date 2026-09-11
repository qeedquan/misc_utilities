/*

Memory partition allows applications to use a separate partition for memory allocations
It allows you to combine memory allocated from somewhere else to be managed by the same parition

*/

#include <vxWorks.h>
#include <stdio.h>
#include <memPartLib.h>

STATUS
test_mempart(void)
{
	STATUS status;
	PART_ID partid;
	char *data;
	char *pool;
	size_t poolsize;
	size_t i;

	status = OK;
	pool = NULL;
	poolsize = 1024;
	partid = memPartCreate(pool, poolsize);
	if (partid == NULL) {
		printf("Failed to create partition\n");
		goto error;
	}

	for (i = 1; i <= 128; i++) {
		poolsize = i * 512;
		pool = malloc(poolsize);
		if (!pool) {
			printf("Failed to allocate memory for partition\n");
			goto error;
		}

		if (memPartAddToPool(partid, pool, poolsize) != OK) {
			printf("Failed to add memory to partition: (%zu bytes)\n", poolsize);
			goto error;
		}
	}

	for (i = 1; i <= 128; i++) {
		poolsize = (i * 512) / 2;
		data = memPartAlloc(partid, poolsize);
		if (!data) {
			printf("Failed to allocate memory: %zu\n", poolsize);
			continue;
		}
		printf("Allocated | Pool Size: %zu Block Size: %zu\n", poolsize, memBlockSizeGet(data));
	}

	if (0) {
	error:
		status = ERROR;
	}

	memPartDelete(partid);
	return status;
}
