#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>

int
main(void)
{
	printf("getpagesize:      %d\n", getpagesize());
	printf("sysconf pagesize: %ld\n", sysconf(_SC_PAGESIZE));

	return 0;
}
