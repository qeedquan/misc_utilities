#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>

typedef unsigned long long uint64;

#define nelem(x) (sizeof(x) / sizeof(x[0]))

typedef struct {
	char name[64];
	clockid_t id;
} clockinfo_t;

typedef struct {
	uint64 ms;
	uint64 sec;
} ticks_t;

clockinfo_t clks[] = {
	{ "CLOCK_REALTIME", CLOCK_REALTIME },
	{ "CLOCK_REALTIME_COARSE", CLOCK_REALTIME_COARSE },
	{ "CLOCK_MONOTONIC", CLOCK_MONOTONIC },
	{ "CLOCK_MONOTONIC_COARSE", CLOCK_MONOTONIC_COARSE },
	{ "CLOCK_MONOTONIC_RAW", CLOCK_MONOTONIC_RAW },
	{ "CLOCK_BOOTTIME", CLOCK_BOOTTIME },
	{ "CLOCK_PROCESS_CPUTIME_ID", CLOCK_PROCESS_CPUTIME_ID },
	{ "CLOCK_THREAD_CPUTIME_ID", CLOCK_THREAD_CPUTIME_ID },
};

void
tickit(ticks_t *p)
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	p->sec = t.tv_sec;
	p->ms = (t.tv_sec * 1000) + (t.tv_nsec / 1000000);
}

void
resolution(void)
{
	struct timespec t;
	size_t i;

	for (i = 0; i < nelem(clks); i++) {
		if (clock_getres(clks[i].id, &t) < 0) {
			perror("clock_getres");
			continue;
		}
		printf("%-32s %ld %ld\n", clks[i].name, (long)t.tv_sec, t.tv_nsec);
	}
}

void
measure(void)
{
	struct timespec t[nelem(clks)] = { 0 }, nt;
	ticks_t c, nc;
	size_t i;

	for (i = 0; i < nelem(clks); i++)
		clock_gettime(clks[i].id, &t[i]);

	tickit(&c);
	printf("\n");
	for (;;) {
		for (i = 0; i < nelem(clks); i++) {
			clock_gettime(clks[i].id, &nt);
			printf("%-32s %ld %ld\n", clks[i].name, (long)(nt.tv_sec - t[i].tv_sec), nt.tv_nsec - t[i].tv_nsec);
		}

		tickit(&nc);
		printf("Tick Seconds      %lld\n", nc.sec - c.sec);
		printf("Tick Milliseconds %lld\n", nc.ms - c.ms);

		printf("\n");
	}
}

int
main(void)
{
	resolution();
	measure();
	return 0;
}
