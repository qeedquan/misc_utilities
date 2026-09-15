/*

Kvaser CAN controller driver

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

enum {
	S5920_INTCSR = 0x38,

	S5920_INTCSR_ADDON_INTENABLE_M = 0x2000,
};

LOCAL const VXB_PCI_DEV_MATCH_ENTRY kvaserpci[] = {
	{ 0x8406, 0x10e8, NULL },
	{},
};

LOCAL void
kvaserfree(SJA1000_CTRL *ctlr)
{
	if (!ctlr)
		return;

	semDelete(ctlr->cmdsem);

	if (ctlr->cookie)
		canDevDisconnect(ctlr->cookie);

	if (ctlr->intres) {
		vxbIntDisconnect(ctlr->dev, ctlr->intres);
		vxbResourceFree(ctlr->dev, ctlr->intres);
	}

	vxbResourceFree(ctlr->dev, ctlr->pcires);
	vxbResourceFree(ctlr->dev, ctlr->adrres);
	vxbDevSoftcSet(ctlr->dev, NULL);
	vxbMemFree(ctlr);
}

LOCAL UINT32
kvaserpciread(SJA1000_CTRL *ctlr, UINT32 reg)
{
	return vxbRead32(ctlr->pcireghandle, (UINT32 *)(ctlr->pciregbase + reg));
}

LOCAL void
kvaserpciwrite(SJA1000_CTRL *ctlr, UINT32 reg, UINT32 val)
{
	vxbWrite32(ctlr->pcireghandle, (UINT32 *)(ctlr->pciregbase + reg), val);
}

LOCAL UINT8
kvasercanread(SJA1000_CTRL *ctlr, UINT8 reg)
{
	return vxbRead8(ctlr->reghandle, (UINT8 *)(ctlr->regbase + (reg * ctlr->regdelta)));
}

LOCAL void
kvasercanwrite(SJA1000_CTRL *ctlr, UINT8 reg, UINT32 val)
{
	vxbWrite8(ctlr->reghandle, (UINT8 *)(ctlr->regbase + (reg * ctlr->regdelta)), val & 0xff);
}

LOCAL STATUS
kvaserprobe(VXB_DEV_ID dev)
{
	return vxbPciDevMatch(dev, kvaserpci, NULL);
}

LOCAL STATUS
kvaserattach(VXB_DEV_ID dev)
{
	SJA1000_CTRL *ctlr;
	VXB_RESOURCE *res, *intres, *pcires;
	VXB_RESOURCE_ADR *resadr, *pciresadr;
	UINT32 value;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (ctlr == NULL)
		goto error;

	ctlr->dev = dev;
	vxbDevSoftcSet(dev, ctlr);

	ctlr->pcires = vxbResourceAlloc(dev, VXB_RES_IO, 0);
	ctlr->adrres = vxbResourceAlloc(dev, VXB_RES_IO, 1);
	ctlr->intres = vxbResourceAlloc(dev, VXB_RES_IRQ, 0);
	if (!ctlr->pcires || !ctlr->adrres || !ctlr->intres)
		goto error;

	res = ctlr->adrres;
	intres = ctlr->intres;
	pcires = ctlr->pcires;

	resadr = res->pRes;
	pciresadr = pcires->pRes;
	if (!resadr || !pciresadr)
		goto error;

	ctlr->reghandle = resadr->pHandle;
	ctlr->regbase = resadr->virtual;
	ctlr->pcireghandle = pciresadr->pHandle;
	ctlr->pciregbase = pciresadr->virtual;

	if (vxbIntConnect(dev, intres, sja1000int, dev) != OK)
		goto error;
	if (vxbIntEnable(dev, intres) != OK)
		goto error;

	SPIN_LOCK_ISR_INIT(&ctlr->spinlock, 0);

	VXB_PCI_BUS_CFG_READ(dev, PCI_CFG_VENDOR_ID, 2, ctlr->pcivid);
	VXB_PCI_BUS_CFG_READ(dev, PCI_CFG_DEVICE_ID, 2, ctlr->pcidid);

	snprintf(ctlr->canname, sizeof(ctlr->canname), "Kvaser CAN Controller");

	ctlr->read = kvasercanread;
	ctlr->write = kvasercanwrite;
	ctlr->xtalfreq = 20 * 1024 * 1024;
	ctlr->regdelta = 1;
	ctlr->mode = 1;
	ctlr->baudrate = 1000;
	ctlr->baudrateset = FALSE;

	if (sja1000create(ctlr) != OK)
		goto error;

	// enable PCI board level interrupts
	value = kvaserpciread(ctlr, S5920_INTCSR);
	value |= S5920_INTCSR_ADDON_INTENABLE_M;
	kvaserpciwrite(ctlr, S5920_INTCSR, S5920_INTCSR_ADDON_INTENABLE_M);

	return OK;

error:
	kvaserfree(ctlr);
	return ERROR;
}

LOCAL STATUS
kvaserdetach(VXB_DEV_ID dev)
{
	SJA1000_CTRL *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	kvaserfree(ctlr);
	return OK;
}

LOCAL STATUS
kvasershutdown(VXB_DEV_ID dev)
{
	void *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	return sja1000shutdown(ctlr);
}

LOCAL VXB_DRV_METHOD kvasermethods[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), kvaserprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), kvaserattach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), kvasershutdown },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), kvaserdetach },
	VXB_DEVMETHOD_END
};

VXB_DRV kvaserdrv = {
	{ NULL },
	"kvaser-can",
	"KVASER CAN PCI Driver",
	VXB_BUSID_PCI,
	VXB_DRVFLAG_RESET,
	0,
	kvasermethods,
	NULL,
};

VXB_DRV_DEF(kvaserdrv)

STATUS
kvaserdrvadd(void)
{
	return vxbDrvAdd(&kvaserdrv);
}
