/*

PL031 RTC timer

DTS:

pl031@9010000 {
    clock-names = "apb_pclk";
    clocks = <0x8000>;
    interrupt-parent = <&intc>;
    interrupts = <0x00 0x02 0x04>;
    reg = <0x00 0x9010000 0x00 0x1000>;
    compatible = "arm,pl031";
};

*/

#include <vxWorks.h>
#include <vxLib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <private/timeP.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <subsys/clk/vxbClkLib.h>
#include <hwif/drv/resource/vxbRtcLib.h>
#include <subsys/timer/vxbTimerLib.h>
#include <subsys/int/vxbIntLib.h>

enum {
	REG_DR = 0x0,
	REG_MR = 0x4,
	REG_LR = 0x8,
	REG_CR = 0xc,
	REG_IMSC = 0x10,
	REG_RIS = 0x14,
	REG_MIS = 0x18,
	REG_ICR = 0x1c,
	REG_PLID = 0xFE0,
};

enum {
	MIN_YEAR = 2000,
	MAX_YEAR = 2099,
};

typedef struct {
	VXB_DEV_ID dev;

	VXB_RESOURCE *res;
	VXB_RESOURCE *intres;

	VIRT_ADDR regbase;
	void *reghandle;

	VXB_I2C_RTC_FUNC rtcfunc;
} PL031_CTLR;

#define csr32r(c, a) vxbRead32(c->reghandle, (UINT32 *)(((char *)(c)->regbase) + ((a))))
#define csr32w(c, a, v) vxbWrite32(c->reghandle, (UINT32 *)(((char *)(c)->regbase) + ((a))), v)

LOCAL const VXB_FDT_DEV_MATCH_ENTRY pl031fdt[] = {
	{ "arm,pl031", NULL },
	{},
};

LOCAL void
pl031free(PL031_CTLR *ctlr)
{
	if (ctlr == NULL)
		return;

	vxbResourceFree(ctlr->dev, ctlr->res);
	vxbResourceFree(ctlr->dev, ctlr->intres);
	vxbMemFree(ctlr);
}

LOCAL void
pl031init(PL031_CTLR *ctlr)
{
	if (!(csr32r(ctlr, REG_CR) & 1))
		csr32w(ctlr, REG_CR, 1);
}

LOCAL STATUS
pl031timeget(VXB_DEV_ID dev, struct tm *rtctime)
{
	PL031_CTLR *ctlr;
	time_t t;

	ctlr = vxbDevSoftcGet(dev);
	t = csr32r(ctlr, REG_DR);
	gmtime_r(&t, rtctime);
	return OK;
}

LOCAL STATUS
pl031timeset(VXB_DEV_ID dev, struct tm *rtctime)
{
	PL031_CTLR *ctlr;
	time_t t;

	ctlr = vxbDevSoftcGet(dev);
	t = mktime(rtctime);
	csr32w(ctlr, REG_DR, t & 0xffffffff);
	return OK;
}

LOCAL STATUS
pl031probe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, pl031fdt, NULL);
}

LOCAL STATUS
pl031attach(VXB_DEV_ID dev)
{
	PL031_CTLR *ctlr;
	VXB_I2C_RTC_FUNC *rf;
	VXB_RESOURCE_ADR *resadr;
	VXB_FDT_DEV *fdtdev;

	if (dev == NULL)
		return ERROR;

	fdtdev = vxbFdtDevGet(dev);
	if (fdtdev == NULL)
		return ERROR;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (ctlr == NULL)
		goto error;

	ctlr->dev = dev;
	ctlr->res = vxbResourceAlloc(dev, VXB_RES_MEMORY, 0);
	ctlr->intres = vxbResourceAlloc(dev, VXB_RES_IRQ, 0);
	if (ctlr->res == NULL || ctlr->intres == NULL)
		goto error;

	resadr = ctlr->res->pRes;
	ctlr->regbase = resadr->virtual;
	ctlr->reghandle = resadr->pHandle;

	rf = &ctlr->rtcfunc;
	rf->rtcGet = pl031timeget;
	rf->rtcSet = pl031timeset;
	rf->minYear = MIN_YEAR - TM_YEAR_BASE;
	rf->maxYear = MAX_YEAR - TM_YEAR_BASE;
	rf->rtcCap = RTC_NEED_PRE_VERIFY;

	vxbClkEnableAll(dev);
	pl031init(ctlr);
	vxbDevSoftcSet(dev, ctlr);
	rtcRegister(dev, rf);

	return OK;

error:
	pl031free(ctlr);
	return ERROR;
}

LOCAL STATUS
pl031shutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL STATUS
pl031detach(VXB_DEV_ID dev)
{
	PL031_CTLR *ctlr;
	ctlr = vxbDevSoftcGet(dev);
	pl031free(ctlr);
	return OK;
}

LOCAL VXB_DRV_METHOD pl031dev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), pl031probe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), pl031attach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), pl031shutdown },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), pl031detach },
	{},
};

VXB_DRV pl031drv = {
	{ NULL },
	"pl031rtc",
	"PL031 real time clock",
	VXB_BUSID_FDT,
	0,
	0,
	pl031dev,
};

VXB_DRV_DEF(pl031drv)

void
pl031link(void)
{
}
