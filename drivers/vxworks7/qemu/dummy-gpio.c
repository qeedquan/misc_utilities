/*

Dummy GPIO driver to test the GPIO API

DTS:

All of the fields are uneeded except compatible but they are there to imitate a real gpio device.

dummy_gpio: dummy_gpio@0 {
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

Other device drivers can reference the GPIO driver shown below.
There is no standard field for "gpios" and is called by various things by other drivers based on the GPIO function.

dummy_gpio_test: dummy_gpio_test@0 {
        compatible = "dummy-gpio-test";
        gpios = <&dummy_gpio 1 2 3 4
                 &dummy_gpio 5 6 7 8
                 &dummy_gpio 9 10 11 12
                 &dummy_gpio 13 14 15 16>;
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>
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

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dgptestfdt[] = {
	{ "dummy-gpio-test", NULL },
	{ NULL },
};

LOCAL STATUS
dgppinalloc(VXB_GPIOCTRL *gpio, UINT32 id)
{
	printf("%s: %p %" PRIx32 "\n", __func__, gpio, id);
	return OK;
}

LOCAL STATUS
dgppinfree(VXB_GPIOCTRL *gpio, UINT32 id)
{
	printf("%s: %p %" PRIx32 "\n", __func__, gpio, id);
	return OK;
}

LOCAL UINT32
dgppingetdir(VXB_GPIOCTRL *gpio, UINT32 id)
{
	printf("%s: %p %" PRIx32 "\n", __func__, gpio, id);
	return 0;
}

LOCAL STATUS
dgppinsetdir(VXB_GPIOCTRL *gpio, UINT32 id, UINT32 dir)
{
	printf("%s: %p %" PRIx32 " %" PRIx32 "\n", __func__, gpio, id, dir);
	return OK;
}

LOCAL UINT32
dgppingetvalue(VXB_GPIOCTRL *gpio, UINT32 id)
{
	printf("%s: %p %" PRIx32 "\n", __func__, gpio, id);
	return 0;
}

LOCAL STATUS
dgppinsetvalue(VXB_GPIOCTRL *gpio, UINT32 id, UINT32 value)
{
	printf("%s: %p %" PRIx32 " %" PRIx32 "\n", __func__, gpio, id, value);
	return OK;
}

LOCAL STATUS
dgpsetdebounce(VXB_GPIOCTRL *gpio, UINT32 id, UINT32 us)
{
	printf("%s: %p %" PRIx32 " %" PRIx32 "\n", __func__, gpio, id, us);
	return OK;
}

LOCAL STATUS
dgpintconfig(VXB_GPIOCTRL *gpio, UINT32 id, INTR_TRIGGER trig, INTR_POLARITY pol)
{
	printf("%s: %p %" PRIx32 " %" PRIx32 " %d %d\n", __func__, gpio, id, trig, pol);
	return OK;
}

LOCAL STATUS
dgpintconnect(VXB_GPIOCTRL *gpio, UINT32 id, VOIDFUNCPTR func, void *arg)
{
	printf("%s: %p %" PRIx32 " %p %p\n", __func__, gpio, id, func, arg);
	return OK;
}

LOCAL STATUS
dgpintdisconnect(VXB_GPIOCTRL *gpio, UINT32 id, VOIDFUNCPTR func, void *arg)
{
	printf("%s: %p %" PRIx32 " %p %p\n", __func__, gpio, id, func, arg);
	return OK;
}

LOCAL STATUS
dgpintenable(VXB_GPIOCTRL *gpio, UINT32 id, VOIDFUNCPTR func, void *arg)
{
	printf("%s: %p %" PRIx32 " %p %p\n", __func__, gpio, id, func, arg);
	return OK;
}

LOCAL STATUS
dgpintdisable(VXB_GPIOCTRL *gpio, UINT32 id, VOIDFUNCPTR func, void *arg)
{
	printf("%s: %p %" PRIx32 " %p %p\n", __func__, gpio, id, func, arg);
	return OK;
}

LOCAL void
dgpshow(VXB_GPIOCTRL *gpio, UINT32 verbose)
{
	printf("%s: %p %" PRId32 "\n", __func__, gpio, verbose);
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

LOCAL int
dgptestprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dgptestfdt, NULL);
}

LOCAL STATUS
dgptestattach(VXB_DEV_ID dev)
{
	int id, i;

	for (i = 0; i < INT_MAX; i++) {
		id = vxbGpioGetByFdtIndex(dev, "gpios", i);
		if (id < 0)
			break;

		printf("gpio index: %d\n", id);
		if (vxbGpioAlloc(id) == OK) {
			printf("gpio alloc: %d\n", id);
			vxbGpioPinRequest(id);
			vxbGpioSetDir(id, 0x0);
			vxbGpioSetValue(id, 0xdead);
			vxbGpioPinRelease(id);
		}
	}

	return OK;
}

LOCAL VXB_DRV_METHOD dgpdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dgpprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dgpattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), dgpdetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), dgpshutdown },
	VXB_DEVMETHOD_END,
};

LOCAL VXB_DRV_METHOD dgpdevtest[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dgptestprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dgptestattach },
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

VXB_DRV dgptestdrv = {
	{ NULL },
	"dummy-gpio-test",
	"Dummy GPIO Test driver",
	VXB_BUSID_FDT,
	0,
	0,
	dgpdevtest,
	NULL,
};

VXB_DRV_DEF(dgpdrv)
VXB_DRV_DEF(dgptestdrv)

STATUS
dgpdrvadd(void)
{
	STATUS status;
	status = vxbDrvAdd(&dgpdrv);
	status |= vxbDrvAdd(&dgptestdrv);
	return status;
}

void
dgpdrvtest(void)
{
	int i;

	for (i = 0; i < 10; i++) {
		vxbGpioPinRequest(i);
		vxbGpioPinRelease(i);
		vxbGpioPinSetDir(i, i);
		vxbGpioSetDir(i, i);
		vxbGpioGetDir(i);
		vxbGpioGetValue(i);
		vxbGpioSetValue(i, i);
		vxbGpioSetDebounce(i, i);
	}
}
