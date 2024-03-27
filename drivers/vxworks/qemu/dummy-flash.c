/*

Dummy Flash driver to test the Flash API

DTS:

dummy_flash: dummy_flash@0 {
    compatible = "dummy_flash";
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <subsys/flash/vxbFlashLib.h>
#include <vxbFlash.h>
#include <vxbFlashCommon.h>

typedef struct {
	VXB_FLASHCTRL flash;
	FLASH_CHIP flashchip;
	VXB_DEV_ID dev;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dflashfdt[] = {
    {"dummy_flash", NULL},
    {NULL},
};

LOCAL void
dflashfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr);
}

LOCAL int
dflashprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dflashfdt, NULL);
}

LOCAL STATUS
dflashattach(VXB_DEV_ID dev)
{
	VXB_FLASHCTRL *flash;
	FLASH_CHIP_ID flashchip;
	Ctlr *ctlr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;

	flashchip = &ctlr->flashchip;
	flashchip->pDev = dev;
	flashchip->uChipSize = 1024 * 16;
	flashchip->uEraseSize = 512;
	flashchip->uFlashType = FLASH_CHIP_TYPE_HYPER;
	snprintf((char *)flashchip->chipName, sizeof(flashchip->chipName), "dummy_flash_chip");

	flash = &ctlr->flash;
	flash->pDev = dev;
	flash->unitId = vxbDevUnitGet(dev);
	flash->flashChip = flashchip;
	snprintf(flash->devName, sizeof(flash->devName), "dummy_flash");

	vxbDevSoftcSet(dev, ctlr);

	if (vxbFlashChipAdd(flash) != OK)
		goto error;

	return OK;

error:
	dflashfree(ctlr);
	return ERROR;
}

LOCAL VXB_DRV_METHOD dflashdev[] = {
    {VXB_DEVMETHOD_CALL(vxbDevProbe), dflashprobe},
    {VXB_DEVMETHOD_CALL(vxbDevAttach), dflashattach},
    VXB_DEVMETHOD_END,
};

VXB_DRV dflashdrv = {
    {NULL},
    "dummy_flash",
    "Dummy Flash driver",
    VXB_BUSID_FDT,
    0,
    0,
    dflashdev,
    NULL,
};

VXB_DRV_DEF(dflashdrv)

STATUS
dflashdrvadd(void)
{
	return vxbDrvAdd(&dflashdrv);
}
