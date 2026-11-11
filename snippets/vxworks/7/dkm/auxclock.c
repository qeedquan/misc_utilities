#include <stdio.h>
#include <stdatomic.h>
#include <vxWorks.h>
#include <sysLib.h>
#include <taskLib.h>

atomic_int aux_counter;

int
auxclock_interrupt(void)
{
	aux_counter++;
	return OK;
}

void
set_auxclock_rate(int rate)
{
	int oldrate, newrate;

	oldrate = sysAuxClkRateGet();
	sysAuxClkRateSet(rate);
	newrate = sysAuxClkRateGet();
	printf("Setting rate: old %llu new %llu\n", oldrate, newrate);
}

int
auxclock_task(int rate, int iterations)
{
	int i, r, v;

	aux_counter = 0;

	r = sysAuxClkConnect(auxclock_interrupt, 0);
	if (r != OK) {
		printf("Failed to connect to the interrupt\n");
		return ERROR;
	}
	sysAuxClkEnable();
	set_auxclock_rate(rate);

	for (i = 1; i <= iterations; i++) {
		taskDelay(sysClkRateGet());
		v = aux_counter;
		printf("%d %d\n", i, v);
	}

	sysAuxClkConnect(NULL, 0);
	sysAuxClkDisable();
	return OK;
}

void
test_auxclock(void)
{
	taskSpawn("Auxillary clock test", 100, VX_FP_TASK | VX_STDIO, 0x4000, auxclock_task, 1000, 100, 0, 0, 0, 0, 0, 0, 0, 0);
}
