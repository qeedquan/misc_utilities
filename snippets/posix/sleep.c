// ./sleep | ts  '%.s' -s
#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <unistd.h>

int
main(void)
{
	struct timespec tp;
	int i;

	setvbuf(stdout, NULL, _IONBF, 0);
	for (i = 0; i < 10; i++) {
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);
		printf("clock_gettime(CLOCK_PROCESS_CPUTIME_ID) %lld %lld\n", (long long)tp.tv_sec, (long long)tp.tv_nsec);

		printf("clock_nanosleep(CLOCK_REALTIME)\n");
		clock_nanosleep(CLOCK_REALTIME, 0, &(struct timespec){0, 500ull * 1000000ull}, NULL);

		printf("clock_nanosleep(CLOCK_MONOTONIC)\n");
		clock_nanosleep(CLOCK_MONOTONIC, 0, &(struct timespec){0, 500ull * 1000000ull}, NULL);

		printf("nanosleep()\n");
		nanosleep(&(struct timespec){0, 500ull * 1000000ull}, NULL);

		printf("sleep()\n");
		sleep(1);
	}

	return 0;
}
