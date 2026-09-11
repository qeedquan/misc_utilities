#include <stdio.h>
#include <memLib.h>
#include <memPartLib.h>
#include <err.h>

void
dump_mem_info(MEM_PART_STATS *m)
{
	printf("Largest Free Block:         %zu\n", memFindMax());
	printf("Number of Bytes Free:       %zu\n", m->numBytesFree);
	printf("Number of Blocks Free:      %zu\n", m->numBlocksFree);
	printf("Max Block Size Free:        %zu\n", m->maxBlockSizeFree);
	printf("Number of Bytes Allocated:  %zu\n", m->numBytesAlloc);
	printf("Number of Blocks Allocated: %zu\n", m->numBlocksAlloc);
	printf("Max Bytes Allocated:        %zu\n", m->maxBytesAlloc);
}

int
main(int argc, char *argv[])
{
	MEM_PART_STATS m;
	
	if (memInfoGet(&m) != OK)
		errx(1, "Failed to get memory information");

	dump_mem_info(&m);
	return 0;
}

