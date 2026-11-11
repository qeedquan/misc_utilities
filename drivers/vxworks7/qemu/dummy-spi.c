/*

Dummy SPI controller driver

DTS:

dummy-spi {
    compatible = "dummy-spi";

    // These fields are required for the SPI child devices specified inside this controller block
    // Without these, resource allocation will fail for the child
    #address-cells = <1>;
    #size-cells = <0>;

    reg = <0x2000 0x1000>;
    clock-frequency = <100000000>;

    interrupts = <1>;
    interrupt-parent = <&dummy_intctrl>;

    clocks = <&spiclk>;
    clock-names = "spiclk";

    // SPI devices that uses this controller, their drivers are implemented somewhere else
    // but they will allocate resources from the controller
    device1@10 {
        compatible = "dummy-spi-device-1";
        reg = <0x10>;
        spi-max-frequency = <5000000>;
        mode = <0x0>;
    };
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <subsys/int/vxbIntLib.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/buslib/vxbFdtSpiLib.h>
#include <hwif/buslib/vxbSpiLib.h>

typedef struct {
	VXB_DEV_ID dev;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dspifdt[] = {
	{ "dummy-spi", NULL },
	{ NULL },
};

LOCAL STATUS
dspixfer(VXB_DEV_ID dev, SPI_HARDWARE *spihw, SPI_TRANSFER *spitx)
{
	printf("%s(dev=%p, spihw=%p, spitx=%p)\n", __func__, dev, spihw, spitx);
	return OK;
}

LOCAL void
dspifree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr);
}

LOCAL int
dspiprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dspifdt, NULL);
}

LOCAL STATUS
dspiattach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	vxbDevSoftcSet(dev, ctlr);

	if (vxbSpiCtrlRegister(dev) != OK)
		goto error;

	return OK;

error:
	dspifree(ctlr);
	return ERROR;
}

LOCAL STATUS
dspidetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	dspifree(ctlr);
	return OK;
}

LOCAL STATUS
dspishutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD dspidev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dspiprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dspiattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), dspidetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), dspishutdown },

	{ VXB_DEVMETHOD_CALL(vxbResourceAlloc), (FUNCPTR)vxbSpiResAlloc },
	{ VXB_DEVMETHOD_CALL(vxbResourceFree), (FUNCPTR)vxbSpiResFree },
	{ VXB_DEVMETHOD_CALL(vxbFdtDevGet), (FUNCPTR)vxbSpiFdtDevGet },
	{ VXB_DEVMETHOD_CALL(vxbSpiXfer), dspixfer },
	VXB_DEVMETHOD_END,
};

VXB_DRV dspidrv = {
	{ NULL },
	"dummy-spi",
	"Dummy SPI driver",
	VXB_BUSID_FDT,
	0,
	0,
	dspidev,
	NULL,
};

VXB_DRV_DEF(dspidrv)

STATUS
dspidrvadd(void)
{
	return vxbDrvAdd(&dspidrv);
}

void
dspilink(void)
{
}
