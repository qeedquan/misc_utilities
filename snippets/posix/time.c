#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

void
timeformats(void)
{
	char buf[80];
	struct tm tm;
	time_t now;

	now = time(NULL);
	gmtime_r(&now, &tm);

	strftime(buf, sizeof(buf), "%FT%TZ", &tm);
	printf("iso8601        %s\n", buf);

	strftime(buf, sizeof(buf), "%Y%m%dT%H%M%SZ", &tm);
	printf("iso8601 (raw)  %s\n", buf);

	asctime_r(&tm, buf);
	printf("asctime        %s", buf);

	ctime_r(&now, buf);
	printf("ctime          %s\n", buf);
}

int
timevalsub(struct timeval *t, struct timeval x, struct timeval y)
{
	long nsec;

	if (x.tv_usec < y.tv_usec) {
		nsec = (y.tv_usec - x.tv_usec) / 1000000 + 1;
		y.tv_usec -= 1000000 * nsec;
		y.tv_sec += nsec;
	}
	if (x.tv_usec - y.tv_usec > 1000000) {
		nsec = (x.tv_usec - y.tv_usec) / 1000000;
		y.tv_usec += 1000000 * nsec;
		y.tv_sec -= nsec;
	}
	t->tv_sec = x.tv_sec - y.tv_sec;
	t->tv_usec = x.tv_usec - y.tv_usec;

	return x.tv_sec < y.tv_sec;
}

void
timediff(void)
{
	struct timeval t, nt, dt;
	int i;

	for (i = 0; i < 10; i++) {
		gettimeofday(&t, NULL);
		sleep(1);
		gettimeofday(&nt, NULL);
		timevalsub(&dt, nt, t);
		printf("%ld %ld\n", dt.tv_sec, dt.tv_usec);
		fflush(stdout);
	}
}

int
main(void)
{
	timeformats();
	timediff();
	return 0;
}
