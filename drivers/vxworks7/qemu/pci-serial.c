/*

16550 serial port over PCI

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <ioLib.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbPciLib.h>
#include <hwif/drv/sio/vxbNs16550Sio.h>

IMPORT STATUS ns16550Create(NS16550VXB_CHAN *chan);
IMPORT STATUS ns16550vxbInt(VXB_DEV_ID dev);

typedef struct {
	// The NS16550 driver expects a NS16550VXB_CHAN* inside a dev so this needs to be a first field
	NS16550VXB_CHAN chans[4];

	VXB_DEV_ID dev;
	VXB_RESOURCE *res;
	VXB_RESOURCE *resint;
	VIRT_ADDR regbase;
	void *reghandle;
	size_t regsize;

	size_t numports;
} Ctlr;

LOCAL const VXB_PCI_DEV_MATCH_ENTRY pcisiomatch[] = {
	{ 0x0002, 0x1b36, (void *)1 },
	{ 0x0003, 0x1b36, (void *)2 },
	{ 0x0004, 0x1b36, (void *)4 },
	{},
};

LOCAL void
pcisiofree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbResourceFree(ctlr->dev, ctlr->res);
	vxbMemFree(ctlr);
}

LOCAL int
pcisioprobe(VXB_DEV_ID dev)
{
	VXB_PCI_DEV_MATCH_ENTRY *match;

	if (vxbPciDevMatch(dev, pcisiomatch, &match) != OK)
		return ERROR;

	vxbDevDrvDataSet(dev, (void *)match->data);
	return OK;
}

LOCAL STATUS
pcisioattach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;
	VXB_RESOURCE_ADR *resadr;
	NS16550VXB_CHAN *chan;
	size_t port;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	ctlr->res = vxbResourceAlloc(dev, VXB_RES_IO, 0);
	if (!ctlr->res)
		goto error;

	ctlr->resint = vxbResourceAlloc(dev, VXB_RES_IRQ, 0);
	if (!ctlr->resint)
		goto error;

	resadr = ctlr->res->pRes;
	ctlr->regbase = resadr->virtual;
	ctlr->reghandle = resadr->pHandle;
	ctlr->regsize = resadr->size;
	ctlr->numports = (size_t)vxbDevDrvDataGet(dev);
	vxbDevSoftcSet(dev, ctlr);

	for (port = 0; port < ctlr->numports; port++) {
		chan = ctlr->chans + port;
		chan->baudRate = 115200;
		chan->xtal = chan->baudRate * 16;
		chan->regDelta = 1;
		chan->fifoSize = NS16550_FIFO_DEPTH_DEFAULT;
		chan->flag = 0;
		chan->pDev = dev;

		if (port == 0) {
			chan->adrRes = ctlr->res;
			chan->intRes = ctlr->resint;
		}

		chan->handle = ctlr->reghandle;
		chan->bar = ctlr->regbase + (port * 8);
		chan->pNext = NULL;
		if (port + 1 < ctlr->numports)
			chan->pNext = ctlr->chans + port + 1;

		ns16550Create(chan);
	}

	if (vxbIntConnect(dev, ctlr->resint, (VOIDFUNCPTR)ns16550vxbInt, dev) != OK)
		goto error;
	if (vxbIntEnable(dev, ctlr->resint) != OK)
		goto error;

	return OK;

error:
	pcisiofree(ctlr);
	return ERROR;
}

LOCAL STATUS
pcisiodetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	pcisiofree(ctlr);
	return OK;
}

LOCAL STATUS
pcisioshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD pcisiodev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), pcisioprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), pcisioattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), pcisiodetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), pcisioshutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV pcisiodrv = {
	{ NULL },
	"qemu-pci-serial",
	"QEMU PCI Serial",
	VXB_BUSID_PCI,
	0,
	0,
	pcisiodev,
	NULL,
};

VXB_DRV_DEF(pcisiodrv)

STATUS
pcisiodrvadd(void)
{
	return vxbDrvAdd(&pcisiodrv);
}

void
pcisiorawtx(const char *str)
{
	VXB_DEV_ID dev;
	Ctlr *ctlr;
	size_t i;

	dev = vxbDevAcquireByName("qemu-pci-serial", 0);
	if (!dev) {
		printf("Failed to get device\n");
		return;
	}

	ctlr = vxbDevSoftcGet(dev);
	for (i = 0; str[i]; i++) {
		vxbWrite8(ctlr->reghandle, (UINT8 *)ctlr->regbase, str[i]);
	}

	vxbDevRelease(dev);
}

void
pcisiochaninfo(char *name, int unit)
{
	VXB_DEV_ID dev;
	NS16550VXB_CHAN *chan;
	int port;

	dev = vxbDevAcquireByName(name, unit);
	if (!dev) {
		printf("Failed to get device\n");
		return;
	}

	printf("%s\n", name);
	chan = vxbDevSoftcGet(dev);
	for (port = 0; chan; port++) {
		printf("Channel #%d\n", port);
		printf("getTxChar %p %p\n", chan->getTxChar, chan->getTxArg);
		printf("putRcvArg %p %p\n", chan->putRcvChar, chan->putRcvArg);
		printf("baudRate %d\n", chan->baudRate);
		printf("flag %#x\n", chan->flag);
		printf("fifoSize %d\n", chan->fifoSize);
		printf("\n");
		chan = chan->pNext;
	}
	printf("\n");

	vxbDevRelease(dev);
}
