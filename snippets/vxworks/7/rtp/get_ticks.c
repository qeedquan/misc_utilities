#include <stdio.h>
#include <tickLib.h>
#include <taskLib.h>

int
main(int argc, char *argv[])
{
	unsigned long long rate;
	unsigned long long tick32, tick64;
	unsigned long long count;

	rate = tickRateGet();
	count = 0;

	printf("Tick rate: %llu\n", rate);
	for (;;) {
		tick32 = tickGet();
		tick64 = tick64Get();
		printf("%llu: %llu %llu\n", count++, tick32, tick64);
		taskDelay(rate);
	}

	return 0;
}
