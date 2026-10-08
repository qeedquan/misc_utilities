#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>

void
maxrdlocks(void)
{
	unsigned long long i;
	pthread_rwlock_t rw;
	int r;

	pthread_rwlock_init(&rw, NULL);

	for (i = 0; i < ULONG_MAX; i++) {
		if ((r = pthread_rwlock_rdlock(&rw)) != 0)
			break;
		if (i % 10000000 == 0)
			printf("%llu\n", i);
	}

	printf("%d %s %llu\n", r, strerror(r), i);
}

int
main(void)
{
	maxrdlocks();
	return 0;
}
