/*

MIOE3680/PCM3680 CAN controller driver

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <spinLockLib.h>
#include <hwif/vxBus.h>
#include <hwif/util/vxbIsrDeferLib.h>
#include <hwif/buslib/vxbPciLib.h>
#include <hwif/buslib/pciDefines.h>
#include <canDevLib.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "sja1000.h"

LOCAL const VXB_PCI_DEV_MATCH_ENTRY can3680pci[] = {
	{ 0xc302, 0x13fe, NULL }, // MIOE3680
	{ 0xc002, 0x13fe, NULL }, // PCM3680
	{},
};

LOCAL void
can3680free(SJA1000_CTRL *ctlr)
{
	SJA1000_CTRL *next;

	if (!ctlr)
		return;

	if (ctlr->intres) {
		vxbIntDisconnect(ctlr->dev, ctlr->intres);
		vxbResourceFree(ctlr->dev, ctlr->intres);
	}

	vxbDevSoftcSet(ctlr->dev, NULL);

	for (; ctlr; ctlr = next) {
		next = ctlr->next;
		if (ctlr->cookie)
			canDevDisconnect(ctlr->cookie);
		semDelete(ctlr->cmdsem);
		vxbResourceFree(ctlr->dev, ctlr->adrres);
		vxbMemFree(ctlr);
	}
}

LOCAL UINT8
can3680canread(SJA1000_CTRL *ctlr, UINT8 reg)
{
	return vxbRead8(ctlr->reghandle, (UINT8 *)(ctlr->regbase + (reg * ctlr->regdelta)));
}

LOCAL void
can3680canwrite(SJA1000_CTRL *ctlr, UINT8 reg, UINT32 val)
{
	vxbWrite8(ctlr->reghandle, (UINT8 *)(ctlr->regbase + (reg * ctlr->regdelta)), val & 0xff);
}

LOCAL STATUS
can3680probe(VXB_DEV_ID dev)
{
	return vxbPciDevMatch(dev, can3680pci, NULL);
}

LOCAL STATUS
can3680attach(VXB_DEV_ID dev)
{
	SJA1000_CTRL *ctlrhead, *ctlrtail, *ctlr;
	VXB_RESOURCE *res, *intres;
	VXB_RESOURCE_ADR *resadr;
	UINT16 i;

	ctlrhead = NULL;
	intres = vxbResourceAlloc(dev, VXB_RES_IRQ, 0);
	for (i = 0; i < 2; i++) {
		ctlr = vxbMemAlloc(sizeof(*ctlr));
		if (!ctlr)
			goto error;

		if (!ctlrhead) {
			ctlrhead = ctlrtail = ctlr;
			vxbDevSoftcSet(dev, ctlrhead);
		} else {
			ctlrtail->next = ctlr;
			ctlrtail = ctlr;
		}

		ctlr->dev = dev;
		ctlr->adrres = vxbResourceAlloc(dev, VXB_RES_IO, i);
		ctlr->intres = intres;
		if (!ctlr->adrres || !ctlr->intres)
			goto error;

		res = ctlr->adrres;
		resadr = res->pRes;
		if (!resadr)
			goto error;

		ctlr->reghandle = resadr->pHandle;
		ctlr->regbase = resadr->virtual;

		SPIN_LOCK_ISR_INIT(&ctlr->spinlock, 0);

		VXB_PCI_BUS_CFG_READ(dev, PCI_CFG_VENDOR_ID, 2, ctlr->pcivid);
		VXB_PCI_BUS_CFG_READ(dev, PCI_CFG_DEVICE_ID, 2, ctlr->pcidid);

		snprintf(ctlr->canname, sizeof(ctlr->canname), "CAN3680 CAN Controller");
		ctlr->read = can3680canread;
		ctlr->write = can3680canwrite;
		ctlr->xtalfreq = 20 * 1024 * 1024;
		ctlr->mode = 1;
		ctlr->baudrate = 1000;
		ctlr->baudrateset = FALSE;
		ctlr->regdelta = 1;
		if (ctlr->pcidid == 0xc302)
			ctlr->regdelta = 4;

		if (sja1000create(ctlr) != OK)
			goto error;
	}

	if (vxbIntConnect(dev, intres, sja1000int, dev) != OK)
		goto error;
	if (vxbIntEnable(dev, intres) != OK)
		goto error;

	return OK;

error:
	can3680free(ctlrhead);
	return ERROR;
}

LOCAL STATUS
can3680detach(VXB_DEV_ID dev)
{
	SJA1000_CTRL *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	can3680free(ctlr);
	return OK;
}

LOCAL STATUS
can3680shutdown(VXB_DEV_ID dev)
{
	void *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	return sja1000shutdown(ctlr);
}

LOCAL VXB_DRV_METHOD can3680methods[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), can3680probe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), can3680attach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), can3680shutdown },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), can3680detach },
	VXB_DEVMETHOD_END
};

VXB_DRV can3680drv = {
	{ NULL },
	"can3680-can",
	"CAN3680 CAN PCI Driver",
	VXB_BUSID_PCI,
	VXB_DRVFLAG_RESET,
	0,
	can3680methods,
	NULL,
};

VXB_DRV_DEF(can3680drv)

STATUS
can3680drvadd(void)
{
	return vxbDrvAdd(&can3680drv);
}
