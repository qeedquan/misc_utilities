/*

Dummy gpio driver to test the gpio API

DTS:

All of the fields are uneeded except compatible but they are there to imitate a real gpio device.

dummy-gpio {
        compatible = "dummy-gpio";
        reg = <0xe6050000 0x50>;
        gpio-controller;
        clocks = <&dummy-clock 55>,
                 <&dummy-clock 205>;
        clock-names = "inputClk", "moduleClk";
        #gpio-cells = <4>;
        interrupts = <36 0 4>;
        interrupt-parent = <&intc>;
        gpio-pinnum = <16>;
        status = "okay";
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <subsys/gpio/vxbGpioLib.h>

typedef struct {
	VXB_GPIOCTRL gpio;
	UINT32 validbmp[32];
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dgpfdt[] = {
	{ "dummy-gpio", NULL },
	{ NULL },
};

LOCAL STATUS
dgppinalloc(VXB_GPIOCTRL *gpio, UINT32 id)
{
	printf("gpio alloc: %p %" PRIx32 "\n", gpio, id);
	return OK;
}

LOCAL STATUS
dgppinfree(VXB_GPIOCTRL *gpio, UINT32 id)
{
	printf("gpio free: %p %" PRIx32 "\n", gpio, id);
	return OK;
}

LOCAL UINT32
dgppingetdir(VXB_GPIOCTRL *gpio, UINT32 id)
{
	printf("gpio getdir: %p %" PRIx32 "\n", gpio, id);
	return 0;
}

LOCAL STATUS
dgppinsetdir(VXB_GPIOCTRL *gpio, UINT32 id, UINT32 dir)
{
	printf("gpio setdir: %p %" PRIx32 " %" PRIx32 "\n", gpio, id, dir);
	return OK;
}

LOCAL UINT32
dgppingetvalue(VXB_GPIOCTRL *gpio, UINT32 id)
{
	printf("gpio getvalue: %p %" PRIx32 "\n", gpio, id);
	return 0;
}

LOCAL STATUS
dgppinsetvalue(VXB_GPIOCTRL *gpio, UINT32 id, UINT32 value)
{
	printf("gpio setdir: %p %" PRIx32 " %" PRIx32 "\n", gpio, id, value);
	return OK;
}

LOCAL STATUS
dgpsetdebounce(VXB_GPIOCTRL *gpio, UINT32 id, UINT32 us)
{
	printf("gpio setdebounce: %p %" PRIx32 " %" PRIx32 "\n", gpio, id, us);
	return OK;
}

LOCAL STATUS
dgpintconfig(VXB_GPIOCTRL *gpio, UINT32 id, INTR_TRIGGER trig, INTR_POLARITY pol)
{
	printf("gpio intconfig: %p %" PRIx32 " %" PRIx32 " %d %d\n", gpio, id, trig, pol);
	return OK;
}

LOCAL STATUS
dgpintconnect(VXB_GPIOCTRL *gpio, UINT32 id, VOIDFUNCPTR func, void *arg)
{
	printf("gpio intconnect: %p %" PRIx32 " %p %p\n", gpio, id, func, arg);
	return OK;
}

LOCAL STATUS
dgpintdisconnect(VXB_GPIOCTRL *gpio, UINT32 id, VOIDFUNCPTR func, void *arg)
{
	printf("gpio intdisconnect: %p %" PRIx32 " %p %p\n", gpio, id, func, arg);
	return OK;
}

LOCAL STATUS
dgpintenable(VXB_GPIOCTRL *gpio, UINT32 id, VOIDFUNCPTR func, void *arg)
{
	printf("gpio intenable: %p %" PRIx32 " %p %p\n", gpio, id, func, arg);
	return OK;
}

LOCAL STATUS
dgpintdisable(VXB_GPIOCTRL *gpio, UINT32 id, VOIDFUNCPTR func, void *arg)
{
	printf("gpio intdisable: %p %" PRIx32 " %p %p\n", gpio, id, func, arg);
	return OK;
}

LOCAL void
dgpshow(VXB_GPIOCTRL *gpio, UINT32 verbose)
{
	printf("gpio show: %p %" PRId32 "\n", gpio, verbose);
}

LOCAL void
dgpfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr);
}

LOCAL int
dgpprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dgpfdt, NULL);
}

LOCAL STATUS
dgpattach(VXB_DEV_ID dev)
{
	VXB_GPIOCTRL *gpio;
	Ctlr *ctlr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	vxbDevSoftcSet(dev, ctlr);

	gpio = &ctlr->gpio;
	gpio->pDev = dev;
	gpio->length = 4;
	gpio->pValidBmp = ctlr->validbmp;
	gpio->gpioAlloc = dgppinalloc;
	gpio->gpioFree = dgppinfree;
	gpio->gpioGetDir = dgppingetdir;
	gpio->gpioSetDir = dgppinsetdir;
	gpio->gpioGetValue = dgppingetvalue;
	gpio->gpioSetValue = dgppinsetvalue;
	gpio->gpioSetDebounce = dgpsetdebounce;
	gpio->gpioIntConnect = dgpintconnect;
	gpio->gpioIntDisConnect = dgpintdisconnect;
	gpio->gpioIntEnable = dgpintenable;
	gpio->gpioIntDisable = dgpintdisable;
	gpio->gpioIntConfig = dgpintconfig;
	gpio->gpioShow = dgpshow;

	if (vxbGpioAddCtlr(gpio) != OK)
		goto error;

	return OK;

error:
	dgpfree(ctlr);
	return ERROR;
}

LOCAL STATUS
dgpdetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	dgpfree(ctlr);
	return OK;
}

LOCAL STATUS
dgpshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD dgpdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dgpprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dgpattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), dgpdetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), dgpshutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV dgpdrv = {
	{ NULL },
	"dummy-gpio",
	"Dummy GPIO driver",
	VXB_BUSID_FDT,
	0,
	0,
	dgpdev,
	NULL,
};

VXB_DRV_DEF(dgpdrv)

STATUS
dgpdrvadd(void)
{
	return vxbDrvAdd(&dgpdrv);
}

void
dgpdrvtest(void)
{
	vxbGpioPinRequest(0);
	vxbGpioPinRelease(0);
	vxbGpioPinSetDir(0, 0);
	vxbGpioSetDir(0, 0);
	vxbGpioGetDir(0);
	vxbGpioGetValue(0);
	vxbGpioSetValue(0, 0);
	vxbGpioSetDebounce(0, 0);
}
