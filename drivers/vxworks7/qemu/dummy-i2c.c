/*

Dummy I2C controller driver

DTS:

dummy-i2c {
    compatible = "dummy-i2c";

    // These fields are required for the I2C child devices specified inside this controller block
    // Without these, resource allocation will fail for the child
    #address-cells = <1>;
    #size-cells = <0>;

    reg = <0x2000 0x1000>;
    clock-frequency = <100000000>;

    interrupts = <1>;
    interrupt-parent = <&dummy_intctrl>;

    // I2C devices that uses this controller, their drivers are implemented somewhere else
    // but they will allocate resources from the I2C controller
    device1@10 {
        compatible = "dummy-i2c-device-1";
        reg = <0x10>;
    };

    device2@20 {
        compatible = "dummy-i2c-device-2";
        reg = <0x23>;
    };
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <subsys/int/vxbIntLib.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/buslib/vxbI2cLib.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/buslib/vxbFdtI2cLib.h>

typedef struct {
	VXB_DEV_ID dev;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY di2cfdt[] = {
	{ "dummy-i2c", NULL },
	{ NULL },
};

LOCAL STATUS
di2cxfer(VXB_DEV_ID dev, I2C_MSG *msgs, int num)
{
	printf("%s(dev=%p, msgs=%p, num=%d)\n", __func__, dev, msgs, num);
	return OK;
}

LOCAL void
di2cfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr);
}

LOCAL int
di2cprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, di2cfdt, NULL);
}

LOCAL STATUS
di2cattach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	vxbDevSoftcSet(dev, ctlr);

	return OK;

error:
	di2cfree(ctlr);
	return ERROR;
}

LOCAL STATUS
di2cdetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	di2cfree(ctlr);
	return OK;
}

LOCAL STATUS
di2cshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_RESOURCE *
di2cresalloc(VXB_DEV_ID dev, VXB_DEV_ID child, UINT32 id)
{
	I2C_DEV_INFO *devinfo;
	VXB_RESOURCE *res;
	VXB_RESOURCE_ADR *resadr;

	devinfo = vxbDevIvarsGet(child);
	res = vxbResourceFind(&devinfo->vxbResList, id);
	if (res == NULL)
		return NULL;

	switch (VXB_RES_TYPE(res->id)) {
	case VXB_RES_MEMORY:
	case VXB_RES_IO:
		resadr = res->pRes;
		resadr->virtual = resadr->start;
		return res;

	case VXB_RES_IRQ:
		if (vxbIntMap(res) == OK)
			return res;
		break;
	}
	return NULL;
}

LOCAL STATUS
di2cresfree(VXB_DEV_ID dev, VXB_DEV_ID child, VXB_RESOURCE *res)
{
	I2C_DEV_INFO *devinfo;

	devinfo = vxbDevIvarsGet(child);
	switch (VXB_RES_TYPE(res->id)) {
	case VXB_RES_MEMORY:
	case VXB_RES_IO:
		return ERROR;
	}
	return vxbResourceRemove(&devinfo->vxbResList, res);
}

LOCAL VXB_RESOURCE_LIST *
di2creslistget(VXB_DEV_ID dev, VXB_DEV_ID child)
{
	I2C_DEV_INFO *devinfo;

	devinfo = vxbDevIvarsGet(child);
	if (devinfo == NULL)
		return NULL;

	return &devinfo->vxbResList;
}

LOCAL VXB_FDT_DEV *
di2cdevget(VXB_DEV_ID dev, VXB_DEV_ID child)
{
	I2C_DEV_INFO *devinfo;

	if (child == NULL)
		return NULL;

	devinfo = vxbDevIvarsGet(child);
	if (devinfo == NULL)
		return NULL;

	return &devinfo->vxbFdtDev;
}

LOCAL VXB_DRV_METHOD di2cdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), di2cprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), di2cattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), di2cdetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), di2cshutdown },

	{ VXB_DEVMETHOD_CALL(vxbResourceAlloc), (FUNCPTR)di2cresalloc },
	{ VXB_DEVMETHOD_CALL(vxbResourceFree), (FUNCPTR)di2cresfree },
	{ VXB_DEVMETHOD_CALL(vxbResourceListGet), (FUNCPTR)di2creslistget },
	{ VXB_DEVMETHOD_CALL(vxbFdtDevGet), (FUNCPTR)di2cdevget },
	{ VXB_DEVMETHOD_CALL(vxbI2cXfer), di2cxfer },
	VXB_DEVMETHOD_END,
};

VXB_DRV di2cdrv = {
	{ NULL },
	"dummy-i2c",
	"Dummy I2C driver",
	VXB_BUSID_FDT,
	0,
	0,
	di2cdev,
	NULL,
};

VXB_DRV_DEF(di2cdrv)

STATUS
di2cdrvadd(void)
{
	return vxbDrvAdd(&di2cdrv);
}

void
di2clink(void)
{
}
