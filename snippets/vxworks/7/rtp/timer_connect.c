#define _GNU_SOURCE
#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <err.h>

void
handler(timer_t tmid, _Vx_usr_arg_t arg)
{
	write(1, "Hello\n", 6);
}

int
main(int argc, char *argv[])
{
	struct itimerspec tv;
	timer_t tmid;

	if (timer_create(CLOCK_REALTIME, NULL, &tmid) == ERROR)
		err(1, "timer_create");

	if (timer_connect(tmid, handler, 0) == ERROR)
		err(1, "timer_connect");

	memset(&tv, 0, sizeof(tv));
	tv.it_interval.tv_sec = 1;
	tv.it_value.tv_nsec = 1;
	if (timer_settime(tmid, CLOCK_REALTIME, &tv, NULL) == ERROR)
		err(1, "timer_settime");

	for (;;)
		sleep(100);

	return 0;
}
