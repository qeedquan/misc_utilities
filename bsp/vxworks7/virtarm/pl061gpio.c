/*

PL061 GPIO Controller

DTS:

pl061@9030000 {
    phandle = <0x8004>;
    clock-names = "apb_pclk";
    clocks = <0x8000>;
    interrupts = <0x00 0x07 0x04>;
    gpio-controller;
    #gpio-cells = <0x02>;
    compatible = "arm,pl061";
    reg = <0x00 0x9030000 0x00 0x1000>;
};

*/

#include <vxWorks.h>
#include <vxLib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <time.h>
#include <private/timeP.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <subsys/clk/vxbClkLib.h>
#include <subsys/timer/vxbTimerLib.h>
#include <subsys/int/vxbIntLib.h>
#include <subsys/gpio/vxbGpioLib.h>

enum {
	REG_DATA = 0x0,
	REG_DIR = 0x400,
	REG_IS = 0x404,
	REG_IBE = 0x408,
	REG_IE = 0x410,
	REG_RIS = 0x414,
	REG_MIS = 0x418,
	REG_IC = 0x41c,
	REG_AFSEL = 0x420,
	REG_PLID = 0xFE0,
};

typedef struct {
	VXB_DEV_ID dev;

	VXB_RESOURCE *res;
	VXB_RESOURCE *intres;

	VIRT_ADDR regbase;
	void *reghandle;

	VXB_GPIOCTRL gpio;
	UINT32 validbmp[256];
} PL061_CTLR;

#define csr32r(c, a) vxbRead32(c->reghandle, (UINT32 *)(((char *)(c)->regbase) + ((a))))
#define csr32w(c, a, v) vxbWrite32(c->reghandle, (UINT32 *)(((char *)(c)->regbase) + ((a))), v)

LOCAL const VXB_FDT_DEV_MATCH_ENTRY pl061fdt[] = {
	{ "arm,pl061", NULL },
	{},
};

LOCAL void
pl061free(PL061_CTLR *ctlr)
{
	if (ctlr == NULL)
		return;

	vxbResourceFree(ctlr->dev, ctlr->res);
	vxbResourceFree(ctlr->dev, ctlr->intres);
	vxbMemFree(ctlr);
}

LOCAL void
pl061init(PL061_CTLR *ctlr)
{
}

LOCAL STATUS
pl061pinalloc(VXB_GPIOCTRL *gpio, UINT32 id)
{
	printf("gpio alloc: %p %" PRIx32 "\n", gpio, id);
	return OK;
}

LOCAL STATUS
pl061pinfree(VXB_GPIOCTRL *gpio, UINT32 id)
{
	printf("gpio free: %p %" PRIx32 "\n", gpio, id);
	return OK;
}

LOCAL UINT32
pl061pingetdir(VXB_GPIOCTRL *gpio, UINT32 id)
{
	printf("gpio getdir: %p %" PRIx32 "\n", gpio, id);
	return 0;
}

LOCAL STATUS
pl061pinsetdir(VXB_GPIOCTRL *gpio, UINT32 id, UINT32 dir)
{
	printf("gpio setdir: %p %" PRIx32 " %" PRIx32 "\n", gpio, id, dir);
	return OK;
}

LOCAL UINT32
pl061pingetvalue(VXB_GPIOCTRL *gpio, UINT32 id)
{
	printf("gpio getvalue: %p %" PRIx32 "\n", gpio, id);
	return 0;
}

LOCAL STATUS
pl061pinsetvalue(VXB_GPIOCTRL *gpio, UINT32 id, UINT32 value)
{
	printf("gpio setdir: %p %" PRIx32 " %" PRIx32 "\n", gpio, id, value);
	return OK;
}

LOCAL STATUS
pl061setdebounce(VXB_GPIOCTRL *gpio, UINT32 id, UINT32 us)
{
	printf("gpio setdebounce: %p %" PRIx32 " %" PRIx32 "\n", gpio, id, us);
	return OK;
}

LOCAL STATUS
pl061intconfig(VXB_GPIOCTRL *gpio, UINT32 id, INTR_TRIGGER trig, INTR_POLARITY pol)
{
	printf("gpio intconfig: %p %" PRIx32 " %" PRIx32 " %d %d\n", gpio, id, trig, pol);
	return OK;
}

LOCAL STATUS
pl061intconnect(VXB_GPIOCTRL *gpio, UINT32 id, VOIDFUNCPTR func, void *arg)
{
	printf("gpio intconnect: %p %" PRIx32 " %p %p\n", gpio, id, func, arg);
	return OK;
}

LOCAL STATUS
pl061intdisconnect(VXB_GPIOCTRL *gpio, UINT32 id, VOIDFUNCPTR func, void *arg)
{
	printf("gpio intdisconnect: %p %" PRIx32 " %p %p\n", gpio, id, func, arg);
	return OK;
}

LOCAL STATUS
pl061intenable(VXB_GPIOCTRL *gpio, UINT32 id, VOIDFUNCPTR func, void *arg)
{
	printf("gpio intenable: %p %" PRIx32 " %p %p\n", gpio, id, func, arg);
	return OK;
}

LOCAL STATUS
pl061intdisable(VXB_GPIOCTRL *gpio, UINT32 id, VOIDFUNCPTR func, void *arg)
{
	printf("gpio intdisable: %p %" PRIx32 " %p %p\n", gpio, id, func, arg);
	return OK;
}

LOCAL void
pl061show(VXB_GPIOCTRL *gpio, UINT32 verbose)
{
	printf("gpio show: %p %" PRId32 "\n", gpio, verbose);
}

LOCAL STATUS
pl061probe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, pl061fdt, NULL);
}

LOCAL STATUS
pl061attach(VXB_DEV_ID dev)
{
	PL061_CTLR *ctlr;
	VXB_GPIOCTRL *gpio;
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

	gpio = &ctlr->gpio;
	gpio->pDev = dev;
	gpio->length = 4;
	gpio->pValidBmp = ctlr->validbmp;
	gpio->gpioAlloc = pl061pinalloc;
	gpio->gpioFree = pl061pinfree;
	gpio->gpioGetDir = pl061pingetdir;
	gpio->gpioSetDir = pl061pinsetdir;
	gpio->gpioGetValue = pl061pingetvalue;
	gpio->gpioSetValue = pl061pinsetvalue;
	gpio->gpioSetDebounce = pl061setdebounce;
	gpio->gpioIntConnect = pl061intconnect;
	gpio->gpioIntDisConnect = pl061intdisconnect;
	gpio->gpioIntEnable = pl061intenable;
	gpio->gpioIntDisable = pl061intdisable;
	gpio->gpioIntConfig = pl061intconfig;
	gpio->gpioShow = pl061show;

	if (vxbGpioAddCtlr(gpio) != OK)
		goto error;

	vxbClkEnableAll(dev);
	pl061init(ctlr);
	vxbDevSoftcSet(dev, ctlr);

	return OK;

error:
	pl061free(ctlr);
	return ERROR;
}

LOCAL STATUS
pl061shutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL STATUS
pl061detach(VXB_DEV_ID dev)
{
	PL061_CTLR *ctlr;
	ctlr = vxbDevSoftcGet(dev);
	pl061free(ctlr);
	return OK;
}

LOCAL VXB_DRV_METHOD pl061dev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), pl061probe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), pl061attach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), pl061shutdown },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), pl061detach },
	{},
};

VXB_DRV pl061drv = {
	{ NULL },
	"pl061gpio",
	"pl061 gpio controller",
	VXB_BUSID_FDT,
	0,
	0,
	pl061dev,
};

VXB_DRV_DEF(pl061drv)

void
pl061link(void)
{
}
