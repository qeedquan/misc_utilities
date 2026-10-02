#include <vxWorks.h>
#include <stdio.h>
#include <subsys/timer/vxbTimerLib.h>

void
test_timestamp(void)
{
	UINT64 period, freq, count;

	vxbTimestampEnable();

	period = vxbTimestampPeriod();
	freq = vxbTimestampFreq();
	count = vxbTimestamp();
	printf("%lu %lu %lu\n", period, freq, count);

	period = vxbTimestamp64Period();
	freq = vxbTimestamp64Freq();
	count = vxbTimestamp64(&count);
	printf("%lu %lu %lu\n", period, freq, count);

	vxbTimestampDisable();
}
