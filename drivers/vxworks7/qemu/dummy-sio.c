/*

Dummy Serial IO driver to test the Serial/I2C/SPI API

DTS:

dummy-sio {
    compatible = "dummy-sio";
};

On real serial devices, the number of trigger bytes for the RX FIFO can be set.
If set too high, the serial console will feel laggy and seem to miss input as it's buffering.
For serial port that are used for the console, it is necessary to set the trigger byte for RX FIFO
to be 1 so it can immediately handle keyboard input.

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <ioLib.h>
#include <ttyLib.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/drv/sio/vxbSioUtil.h>
#include <hwif/buslib/vxbFdtSpiLib.h>
#include <hwif/buslib/vxbI2cLib.h>

typedef STATUS (*gettxcharfn)(void *, char *);
typedef void (*putrcvcharfn)(void *, char);

typedef struct {
	// must be first field
	SIO_DRV_FUNCS *serfuncs;

	char ttyname[TTY_MAX_NAME_LENGTH];
	int ttyindex;
	int ttychannel;
	UINT baudrate;
	UINT mode;
	UINT options;

	gettxcharfn gettxchar;
	putrcvcharfn putrcvchar;
	void *gettxarg;
	void *putrcvarg;

	VXB_DEV_ID dev;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dsiofdt[] = {
	{ "dummy-sio", NULL },
	{ NULL },
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

LOCAL int
serioctl(SIO_CHAN *chan, int req, void *arg)
{
	Ctlr *ctlr;

	printf("%s(chan=%p, req=%d, arg=%p)\n", __func__, chan, req, arg);

	ctlr = (Ctlr *)chan;
	switch (req) {
	case SIO_NAME_GET:
		snprintf(arg, TTY_MAX_NAME_LENGTH, "%s", ctlr->ttyname);
		break;

	case SIO_BAUD_SET:
		ctlr->baudrate = (UINT)arg;
		break;

	case SIO_BAUD_GET:
		*(UINT *)arg = ctlr->baudrate;
		break;

	case SIO_MODE_SET:
		ctlr->mode = (UINT)arg;
		break;

	case SIO_MODE_GET:
		*(UINT *)arg = ctlr->mode;
		break;

	case SIO_AVAIL_MODES_GET:
		*(int *)arg = SIO_MODE_INT | SIO_MODE_POLL;
		break;

	case SIO_HW_OPTS_SET:
		ctlr->options = (UINT)arg;
		break;

	case SIO_HW_OPTS_GET:
		*(UINT *)arg = ctlr->options;
		break;

	case SIO_DEV_LOCK:
		break;

	case SIO_DEV_UNLOCK:
		break;

	default:
		return ENOSYS;
	}
	return OK;
}

LOCAL int
sertxstartup(SIO_CHAN *chan)
{
	printf("%s(chan=%p)\n", __func__, chan);
	return OK;
}

LOCAL int
sercallbackinstall(SIO_CHAN *chan, int type, STATUS (*func)(void *, ...), void *arg)
{
	Ctlr *ctlr;

	printf("%s(chan=%p, type=%d, func=%p, arg=%p)\n", __func__, chan, type, func, arg);
	ctlr = (Ctlr *)chan;
	switch (type) {
	case SIO_CALLBACK_GET_TX_CHAR:
		ctlr->gettxchar = (gettxcharfn)func;
		ctlr->gettxarg = arg;
		break;

	case SIO_CALLBACK_PUT_RCV_CHAR:
		ctlr->putrcvchar = (putrcvcharfn)func;
		ctlr->putrcvarg = arg;
		break;

	default:
		return ENOSYS;
	}
	return OK;
}

LOCAL int
serpollinput(SIO_CHAN *chan, char *out)
{
	printf("%s(chan=%p, out=%p)\n", __func__, chan, out);
	return OK;
}

LOCAL int
serpolloutput(SIO_CHAN *chan, char out)
{
	printf("%s(chan=%p, out=%d)\n", __func__, chan, out);
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
	static SIO_DRV_FUNCS serfuncs = {
		serioctl,
		sertxstartup,
		sercallbackinstall,
		serpollinput,
		serpolloutput,
	};

	Ctlr *ctlr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	ctlr->serfuncs = &serfuncs;

	if (vxbSpiCtrlRegister(dev) != OK)
		goto error;

	ctlr->ttychannel = vxbSioNextGet();
	vxbDevSoftcSet(dev, ctlr);

#ifdef _WRS_CONFIG_CORE__IO_V2
	usrTtyDevCreate(TTY_DEV_ONBOARD, &ctlr->ttyindex, ctlr->ttyname, (SIO_CHAN *)ctlr,
	    VXB_SIO_RX_BUF_SIZE, VXB_SIO_TX_BUF_SIZE, ctlr->ttychannel);
#else
	ctlr->ttyindex = ctlr->ttychannel;
	ttyDevCreate(ctrl->ttyname, ctlr, VXB_SIO_RX_BUF_SIZE, VXB_SIO_TX_BUF_SIZE);
#endif

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
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dsioprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dsioattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), dsiodetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), dsioshutdown },

	{ VXB_DEVMETHOD_CALL(vxbI2cXfer), i2cxfer },
	{ VXB_DEVMETHOD_CALL(vxbSpiXfer), spixfer },
	VXB_DEVMETHOD_END,
};

VXB_DRV dsiodrv = {
	{ NULL },
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
