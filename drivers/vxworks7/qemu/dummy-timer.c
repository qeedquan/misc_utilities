/*

Dummy timer driver to test the timer API

DTS:

dummy-timer {
    compatible = "dummy-timer";

    // default frequency of the clock
    clock-frequency = <101010101>;
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <subsys/timer/vxbTimerLib.h>
#include <subsys/clk/vxbClkLib.h>

typedef struct {
	struct vxbTimerFunctionality timerfunc;
	UINT64 freq;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dtmfdt[] = {
	{ "dummy-timer", NULL },
	{ NULL },
};

LOCAL STATUS
tmallocate(void *cookie, UINT32 flags)
{
	printf("timer allocate: %p %#" PRIx32 "\n", cookie, flags);
	return OK;
}

LOCAL STATUS
tmrelease(void *cookie)
{
	printf("timer release: %p\n", cookie);
	return OK;
}

LOCAL STATUS
tmrolloverget(void *cookie, UINT32 *count)
{
	printf("timer rollover get: %p %p\n", cookie, count);
	return OK;
}

LOCAL STATUS
tmcountget(void *cookie, UINT32 *count)
{
	printf("timer count get: %p %p\n", cookie, count);
	return OK;
}

LOCAL STATUS
tmdisable(void *cookie)
{
	printf("timer disable: %p\n", cookie);
	return OK;
}

LOCAL STATUS
tmenable(void *cookie, UINT32 maxcount)
{
	printf("timer disable: %p %" PRId32 "\n", cookie, maxcount);
	return OK;
}

LOCAL STATUS
tmisrset(void *cookie, void (*func)(_Vx_usr_arg_t), _Vx_usr_arg_t arg)
{
	printf("timer isr set: %p %p %lx\n", cookie, func, (unsigned long)arg);
	return OK;
}

LOCAL STATUS
tmenable64(void *cookie, UINT64 maxcount)
{
	printf("timer disable 64: %p %" PRId64 "\n", cookie, maxcount);
	return OK;
}

LOCAL STATUS
tmrolloverget64(void *cookie, UINT64 *count)
{
	printf("timer rollover get 64: %p %p\n", cookie, count);
	return OK;
}

LOCAL STATUS
tmcountget64(void *cookie, UINT64 *count)
{
	printf("timer count get 64: %p %p\n", cookie, count);
	return OK;
}

LOCAL STATUS
tmdeadlineenable(void *cookie, UINT64 deadline, UINT64 maxcount)
{
	printf("timer deadline enable: %p %" PRId64 "%" PRId64 "\n", cookie, deadline, maxcount);
	return OK;
}

LOCAL void
dtmfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr);
}

LOCAL int
dtmprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dtmfdt, NULL);
}

LOCAL STATUS
dtmattach(VXB_DEV_ID dev)
{
	struct vxbTimerFunctionality *tf;
	VXB_CLK_ID clk;
	Ctlr *ctlr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	if (vxbClkEnableAll(dev) == ERROR)
		goto error;

	clk = vxbClkGet(dev, NULL);
	ctlr->freq = vxbClkRateGet(clk);

	tf = &ctlr->timerfunc;
	snprintf(tf->timerName, sizeof(tf->timerName), "dummy-timer");
	tf->features = VXB_TIMER_CAN_INTERRUPT |
	               VXB_TIMER_INTERMEDIATE_COUNT |
	               VXB_TIMER_SIZE_32 |
	               VXB_TIMER_SIZE_64 |
	               VXB_TIMER_AUTO_RELOAD |
	               VXB_TIMER_CAN_INTERRUPT;
	tf->rolloverPeriod = ~tf->rolloverPeriod;
	tf->minFrequency = 1;
	tf->maxFrequency = ctlr->freq;
	tf->timerAllocate = tmallocate;
	tf->timerRelease = tmrelease;
	tf->timerRolloverGet = tmrolloverget;
	tf->timerCountGet = tmcountget;
	tf->timerDisable = tmdisable;
	tf->timerEnable = tmenable;
	tf->timerISRSet = tmisrset;
	tf->timerEnable64 = tmenable64;
	tf->timerRolloverGet64 = tmrolloverget64;
	tf->timerCountGet64 = tmcountget64;
	tf->timerDeadlineEnable = tmdeadlineenable;
	vxbTimerRegister(tf);

	vxbDevSoftcSet(dev, ctlr);

	return OK;

error:
	dtmfree(ctlr);
	return ERROR;
}

LOCAL STATUS
dtmdetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	dtmfree(ctlr);
	return OK;
}

LOCAL STATUS
dtmshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD dtmdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dtmprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dtmattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), dtmdetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), dtmshutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV dtmdrv = {
	{ NULL },
	"dummy-timer",
	"Dummy timer driver",
	VXB_BUSID_FDT,
	0,
	0,
	dtmdev,
	NULL,
};

VXB_DRV_DEF(dtmdrv)

STATUS
dtmdrvadd(void)
{
	return vxbDrvAdd(&dtmdrv);
}

LOCAL int
tmeval(timerHandle_t candidate)
{
	UINT32 minfreq, maxfreq, features;

	vxbTimerFeaturesGet(candidate, &minfreq, &maxfreq, &features);

	printf("timer eval: %p | min freq: %" PRId32 " max freq %" PRId32 " features %#" PRIx32 "\n",
	    candidate, minfreq, maxfreq, features);
	return 1;
}

void
dtmdrvtest(void)
{
	vxbSysClkShow();
	vxbAuxClkShow();
	vxbTimestampShow();
	vxbTimerEval(tmeval);
}
