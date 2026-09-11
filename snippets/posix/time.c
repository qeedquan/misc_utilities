#define _GNU_SOURCE
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

void
timeformats(void)
{
	static const char *timefmts[][2] = {
		{ "%Y%m%d%H%M%S", "yyyymmddhhmmss" },
		{ "%Y.%m.%d.%H.%M.%S", "yyyy.mm.dd.hh.mm.ss" },
		{ "%y%m%d%H%M%S", "yymmddhhmmss" },
		{ "%y.%m.%d.%H.%M.%S", "yy.mm.dd.hh.mm.ss" },
		{ "%y%m%d%H%M", "yymmddhhmm" },
		{ "%H%M%S", "hhmmss" },
		{ "%c", "locale" },
		{ "%Y-%m-%dT%H:%M:%S", "ISO 8601 date + time" },
		{ "%Y-%m-%dT%H:%M", "ISO 8601 date + hh:mm" },
		{ "%Y%m%d%H%M%S", "ISO 8601 date + time, basic" },
		{ "%Y%m%d%H%M", "ISO 8601 date + hh:mm, basic" },
		{ "%FT%TZ", "ISO8601" },
	};

	char buf[128];
	struct tm tm;
	time_t now;
	size_t i;

	now = time(NULL);
	for (i = 0; i < nelem(timefmts); i++) {
		gmtime_r(&now, &tm);
		strftime(buf, sizeof(buf), timefmts[i][0], &tm);
		printf("%-32s %-32s\n", timefmts[i][1], buf);

		localtime_r(&now, &tm);
		strftime(buf, sizeof(buf), timefmts[i][0], &tm);
		printf("%-32s %-32s\n", timefmts[i][1], buf);

		printf("\n");
	}

	asctime_r(&tm, buf);
	printf("%-32s %-32s\n", "ASCTIME", buf);

	ctime_r(&now, buf);
	printf("%-32s %-32s\n", "CTIME", buf);
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
