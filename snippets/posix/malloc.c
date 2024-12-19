#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <mcheck.h>

void
dumpmallinfo(void)
{
	struct mallinfo mi;

	mi = mallinfo();
	printf("\n");
	printf("Total non-mmapped bytes (arena):       %d\n", mi.arena);
	printf("# of free chunks (ordblks):            %d\n", mi.ordblks);
	printf("# of free fastbin blocks (smblks):     %d\n", mi.smblks);
	printf("# of mapped regions (hblks):           %d\n", mi.hblks);
	printf("Bytes in mapped regions (hblkhd):      %d\n", mi.hblkhd);
	printf("Max. total allocated space (usmblks):  %d\n", mi.usmblks);
	printf("Free bytes held in fastbins (fsmblks): %d\n", mi.fsmblks);
	printf("Total allocated space (uordblks):      %d\n", mi.uordblks);
	printf("Total free space (fordblks):           %d\n", mi.fordblks);
	printf("Topmost releasable block (keepcost):   %d\n", mi.keepcost);
	printf("\n");
}

int
main(void)
{
	int i;

	setenv("MALLOC_TRACE", "/dev/stdout", 0);
	mtrace();
	for (i = 0; i < 1024; i++)
		free(malloc(1024 * (rand() % 1024)));
	malloc_stats();
	malloc_info(0, stdout);
	dumpmallinfo();
	printf("trim %d\n", malloc_trim(1024));
	muntrace();
	return 0;
}
