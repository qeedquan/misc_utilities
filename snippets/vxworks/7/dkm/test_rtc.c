#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdatomic.h>
#include <sysLib.h>
#include <hwif/drv/resource/vxbRtcLib.h>

atomic_int alarmcount;

STATUS
alarmfunc(void *arg)
{
	kprintf("ALARM TRIGGERED %p\n", arg);
	alarmcount++;
	return OK;
}

void
test_alarm(int duration)
{
	struct tm tm;
	int minyear, maxyear;

	if (duration < 1 || duration >= 60) {
		printf("Invalid duration\n");
		return;
	}

	minyear = vxbRtcMinYearGet();
	maxyear = vxbRtcMaxYearGet();
	for (;;) {
		if (vxbRtcGet(&tm) != OK) {
			printf("Failed to get RTC time\n");
			return;
		}
		if (tm.tm_sec + duration < 60) {
			tm.tm_sec += duration;
			break;
		}
		sleep(1);
	}

	if (vxbRtcTimeVerify(&tm, minyear, maxyear) != OK) {
		printf("Invalid time specified\n");
		return;
	}

	printf("RTC Available: %d\n", vxbRtcAvailable());
	printf("RTC Cap: %#x\n", vxbRtcCapGet());
	printf("Min Year %d Max Year: %d\n", minyear, maxyear);
	printf("Alarm Number: %d\n", vxbAlarmNumGet());
	if (vxbAlarmSet(0, &tm, alarmfunc, (void *)12345) != OK)
		printf("Failed to set alarm\n");

	printf("Sleeping for %d seconds\n", duration);
	sleep(duration);
	printf("Alarm Count: %d\n", alarmcount);
}
