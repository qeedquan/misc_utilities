/*

Dummy Serial IO driver to test the I2C/SPI API

DTS:

dummy-sio {
    compatible = "dummy-sio";
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/buslib/vxbFdtSpiLib.h>
#include <hwif/buslib/vxbI2cLib.h>

typedef struct {
	VXB_DEV_ID dev;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dsiofdt[] = {
    {"dummy-sio", NULL},
    {NULL},
};

LOCAL STATUS
i2cxfer(VXB_DEV_ID dev, I2C_MSG *msgs, int num)
{
	printf("%s(dev=%p, msgs=%p, num=%d)\n", __func__, dev, msgs, num);
	return OK;
}

LOCAL STATUS
spixfer(VXB_DEV_ID dev, SPI_HARDWARE *hardware, SPI_TRANSFER *transfer)
{
	printf("%s(dev=%p, hardware=%p, transfer=%p)\n", __func__, dev, hardware, transfer);
	return OK;
}

LOCAL void
dsiofree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr);
}

LOCAL int
dsioprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dsiofdt, NULL);
}

LOCAL STATUS
dsioattach(VXB_DEV_ID dev)
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
	dsiofree(ctlr);
	return ERROR;
}

LOCAL STATUS
dsiodetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	dsiofree(ctlr);
	return OK;
}

LOCAL STATUS
dsioshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD dsiodev[] = {
    {VXB_DEVMETHOD_CALL(vxbDevProbe), dsioprobe},
    {VXB_DEVMETHOD_CALL(vxbDevAttach), dsioattach},
    {VXB_DEVMETHOD_CALL(vxbDevDetach), dsiodetach},
    {VXB_DEVMETHOD_CALL(vxbDevShutdown), dsioshutdown},

    {VXB_DEVMETHOD_CALL(vxbI2cXfer), i2cxfer},
    {VXB_DEVMETHOD_CALL(vxbSpiXfer), spixfer},
    VXB_DEVMETHOD_END,
};

VXB_DRV dsiodrv = {
    {NULL},
    "dummy-sio",
    "Dummy SIO driver",
    VXB_BUSID_FDT,
    0,
    0,
    dsiodev,
    NULL,
};

VXB_DRV_DEF(dsiodrv)

STATUS
dsiodrvadd(void)
{
	return vxbDrvAdd(&dsiodrv);
}

void
dsiodrvtest(void)
{
	VXB_DEV_ID dev;
	I2C_MSG i2cmsg;
	SPI_HARDWARE spihw;
	SPI_TRANSFER spitx;
	UINT8 buf[32];

	dev = vxbDevAcquireByName("dummy-sio", 0);
	if (dev == NULL)
		return;

	memset(&i2cmsg, 0, sizeof(i2cmsg));
	memset(&spihw, 0, sizeof(spihw));
	memset(&spitx, 0, sizeof(spitx));

	i2cmsg.addr = 0x39;
	i2cmsg.buf = buf;
	i2cmsg.len = sizeof(buf);
	i2cmsg.flags = I2C_M_WR;
	VXB_I2C_XFER(dev, &i2cmsg, 1);
	VXB_SPI_XFER(dev, &spihw, &spitx);

	vxbDevRelease(dev);
}
