#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <pthread.h>

void
runner(union sigval sv)
{
	int *cnt;

	cnt = sv.sival_ptr;
	*cnt += 1;

	printf("%d\n", *cnt);
	fflush(stdout);
}

int
main(int argc, char *argv[])
{
	timer_t ti;
	struct sigevent sev;
	struct itimerspec it;
	pthread_attr_t attr;
	int cnt;

	memset(&sev, 0, sizeof(sev));
	memset(&it, 0, sizeof(it));
	cnt = 0;

	if (pthread_attr_init(&attr) != 0)
		errx(1, "pthread_attr_init");

	if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0)
		errx(1, "pthread_attr_setdetachstate");

	sev.sigev_notify = SIGEV_THREAD;
	sev.sigev_signo = SIGRTMIN;
	sev.sigev_value.sival_ptr = &cnt;
	sev.sigev_notify_function = runner;
	sev.sigev_notify_attributes = &attr;

	it.it_interval.tv_nsec = 10000000ull;
	it.it_value.tv_sec = 1;

	if (timer_create(CLOCK_MONOTONIC, &sev, &ti) < 0)
		err(1, "timer_create");

	if (timer_settime(ti, 0, &it, NULL) < 0)
		err(1, "timer_settime");

	for (;;)
		sleep(1000);

	timer_delete(ti);

	return 0;
}
