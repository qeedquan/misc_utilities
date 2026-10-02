/*

Dummy RTC driver to test the RTC API

DTS:

dummy-rtc {
    compatible = "dummy-rtc";

    // seconds since epoch time
    seconds = <0x12345678 0x0>;
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <private/timeP.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/drv/resource/vxbRtcLib.h>

typedef struct {
	VXB_I2C_RTC_FUNC rtc;
	int64_t sec;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY drtcfdt[] = {
	{ "dummy-rtc", NULL },
	{ NULL },
};

LOCAL STATUS
rtcget(VXB_DEV_ID dev, struct tm *tm)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	return vxbRtcGmtime(ctlr->sec, tm);
}

LOCAL STATUS
rtcset(VXB_DEV_ID dev, struct tm *tm)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	ctlr->sec = vxbRtcTimegm(tm);
	return OK;
}

LOCAL STATUS
alarmset(VXB_DEV_ID dev, UINT8 slot, struct tm *tm, RTC_ALARM_FUNC func, void *arg)
{
	return OK;

	(void)dev;
	(void)slot;
	(void)tm;
	(void)func;
	(void)arg;
}

LOCAL STATUS
alarmget(VXB_DEV_ID dev, UINT8 slot, struct tm *tm)
{
	return OK;

	(void)dev;
	(void)slot;
	(void)tm;
}

LOCAL void
drtcfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr);
}

LOCAL int
drtcprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, drtcfdt, NULL);
}

LOCAL STATUS
drtcattach(VXB_DEV_ID dev)
{
	VXB_FDT_DEV *fdtdev;
	VXB_I2C_RTC_FUNC *rtc;
	Ctlr *ctlr;
	const UINT64 *prop;
	int proplen;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	fdtdev = vxbFdtDevGet(dev);
	if (!ctlr || !fdtdev)
		goto error;

	vxbDevSoftcSet(dev, ctlr);

	prop = vxFdtPropGet(fdtdev->offset, "seconds", &proplen);
	if (prop && proplen == 8)
		ctlr->sec = vxFdt64ToCpu(*prop);

	rtc = &ctlr->rtc;
	rtc->rtcGet = rtcget;
	rtc->rtcSet = rtcset;
	rtc->alarmSet = alarmset;
	rtc->alarmGet = alarmget;
	rtc->alarmNum = 0;
	rtc->maxYear = 2099 - TM_YEAR_BASE;
	rtc->minYear = 1970 - TM_YEAR_BASE;
	rtc->rtcCap = RTC_NEED_PRE_VERIFY;
	snprintf(rtc->timerName, sizeof(rtc->timerName), "dummy-rtc");
	rtcRegister(dev, rtc);

	return OK;

error:
	drtcfree(ctlr);
	return ERROR;
}

LOCAL STATUS
drtcdetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	drtcfree(ctlr);
	return OK;
}

LOCAL STATUS
drtcshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD drtcdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), drtcprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), drtcattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), drtcdetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), drtcshutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV drtcdrv = {
	{ NULL },
	"dummy-rtc",
	"Dummy RTC driver",
	VXB_BUSID_FDT,
	0,
	0,
	drtcdev,
	NULL,
};

VXB_DRV_DEF(drtcdrv)

STATUS
drtcdrvadd(void)
{
	return vxbDrvAdd(&drtcdrv);
}

void
drtcdrvtest(void)
{
	printf("RTC\n");
	printf("\tAvailable:  %d\n", vxbRtcAvailable());
	printf("\tCapability: %" PRIx32 "\n", vxbRtcCapGet());
	printf("\tMin Year:   %d\n", TM_YEAR_BASE + vxbRtcMinYearGet());
	printf("\tMax Year:   %d\n", TM_YEAR_BASE + vxbRtcMaxYearGet());
	printf("\tAlarms:     %d\n", vxbAlarmNumGet());
	printf("\tAlarm Type: %d\n", vxbAlarmTypeCheck());
	sysRtcGet(1);
}
