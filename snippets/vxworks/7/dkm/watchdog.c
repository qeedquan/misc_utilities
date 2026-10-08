/*

Tests watchdog api by rearming the watchdog to make a repeating timer callback until it gets canceled
This is not a hardware watchdog that resets the board but more of a software watchdog that uses the system clock to handle alarms

*/

#include <vxWorks.h>
#include <unistd.h>
#include <logLib.h>
#include <wdLib.h>
#include <sysLib.h>

WDOG_ID wdid;

// this keeps rearming the watchdog until it is canceled
int
wdtimer(void)
{
	logMsg("Watchdog triggered\n", 0, 0, 0, 0, 0, 0);
	wdStart(wdid, sysClkRateGet() * 1, wdtimer, 0);
	return OK;
}

STATUS
wdtask(void)
{
	wdid = wdCreate();
	if (!wdid)
		return ERROR;

	wdShow(wdid);
	wdStart(wdid, sysClkRateGet() * 1, wdtimer, 0);
	wdShow(wdid);

	sleep(5);

	wdCancel(wdid);
	wdDelete(wdid);

	return OK;
}
