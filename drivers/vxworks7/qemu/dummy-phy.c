/*

Dummy phy driver to test the phy API

DTS:

dummy_phy: dummy_phy@0 {
    compatible = "dummy_phy";

    // number of phy parameters for this module
    #phy-cells = <0>;
};

// a device that uses the dummy_phy
dummy_phy_test {
    phys = <&dummy_phy>;
    phy-names = "dummy_phy";
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <subsys/phy/vxbPhyLib.h>

typedef struct {
	VXB_PHY *phy;
	VXB_DEV_ID dev;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dphyfdt[] = {
	{ "dummy_phy", NULL },
	{ NULL },
};

LOCAL STATUS
phytranslate(VXB_DEV_ID dev, PHY_TRANS_ARGS *phytrans, void *ctx, VXB_PHY_ID *phyid)
{
	Ctlr *ctlr;

	printf("phy translate: dev=%p phytrans=%p ctx=%p phyid=%p\n", dev, phytrans, ctx, phyid);
	ctlr = vxbDevSoftcGet(dev);
	*phyid = ctlr->phy;
	return OK;
}

LOCAL STATUS
phyinit(VXB_PHY_ID phy)
{
	printf("phy init: %p\n", phy);
	return OK;
}

LOCAL STATUS
phyreset(VXB_PHY_ID phy)
{
	printf("phy reset: %p\n", phy);
	return OK;
}

LOCAL STATUS
phypoweron(VXB_PHY_ID phy)
{
	printf("phy poweron: %p\n", phy);
	return OK;
}

LOCAL STATUS
phypoweroff(VXB_PHY_ID phy)
{
	printf("phy poweroff: %p\n", phy);
	return OK;
}

LOCAL STATUS
phyrelease(VXB_PHY_ID phy)
{
	printf("phy release: %p\n", phy);
	return OK;
}

LOCAL STATUS
phyexit(VXB_PHY_ID phy)
{
	printf("phy exit: %p\n", phy);
	return OK;
}

LOCAL STATUS
phymodeset(VXB_PHY_ID phy, PHY_MODE mode, int submode)
{
	printf("phy exit: %p mode=%d submode=%d\n", phy, mode, submode);
	return OK;
}

LOCAL STATUS
phyconfigure(VXB_PHY_ID phy, void *cfg)
{
	printf("phy configure: phy=%p cfg=%p\n", phy, cfg);
	return OK;
}

LOCAL STATUS
phyvalidate(VXB_PHY_ID phy, PHY_MODE mode, int submode, void *cfg)
{
	printf("phy validate: phy=%p mode=%d submode=%d cfg=%p\n", phy, mode, submode, cfg);
	return OK;
}

LOCAL void
dphyfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbPhyDestroy(ctlr->phy);
	vxbMemFree(ctlr);
}

LOCAL int
dphyprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dphyfdt, NULL);
}

LOCAL STATUS
dphyattach(VXB_DEV_ID dev)
{
	static VXB_PHY_FUNC phyfunc = {
		.release = phyrelease,
		.reset = phyreset,
		.init = phyinit,
		.powerOn = phypoweron,
		.powerOff = phypoweroff,
		.exit = phyexit,
		.modeSet = phymodeset,
		.configure = phyconfigure,
		.validate = phyvalidate,
	};

	Ctlr *ctlr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->phy = vxbMemAlloc(sizeof(*ctlr->phy));
	if (!ctlr->phy)
		goto error;

	ctlr->dev = dev;
	vxbDevSoftcSet(dev, ctlr);

	if (vxbPhyCreate(dev, &ctlr->phy, NULL, &phyfunc) != OK)
		goto error;

	if (vxbPhyProviderRegister(dev, phytranslate, NULL) == NULL)
		goto error;

	return OK;

error:
	dphyfree(ctlr);
	return ERROR;
}

LOCAL STATUS
dphydetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	dphyfree(ctlr);
	return OK;
}

LOCAL STATUS
dphyshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD dphydev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dphyprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dphyattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), dphydetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), dphyshutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV dphydrv = {
	{ NULL },
	"dummy_phy",
	"Dummy phy driver",
	VXB_BUSID_FDT,
	0,
	0,
	dphydev,
	NULL,
};

VXB_DRV_DEF(dphydrv)

STATUS
dphydrvadd(void)
{
	return vxbDrvAdd(&dphydrv);
}

void
dphydrvtest(void)
{
	VXB_DEV_ID dev;
	VXB_PHY_ID phy;

	dev = vxbDevAcquireByName("dummy_phy_test", 0);
	if (dev == NULL) {
		printf("failed to get phy device\n");
		return;
	}

	if (vxbPhyGet(dev, "dummy_phy", &phy) != OK) {
		printf("failed to get phy id\n");
	}

	vxbPhyInit(phy);
	vxbPhyReset(phy);
	vxbPhyPowerOn(phy);
	vxbPhyPowerOff(phy);
	vxbPhyModeSet(phy, PHY_MODE_PCIE, 1);
	vxbPhyRelease(phy);
	printf("data: %p\n", vxbPhyDrvDataGet(phy));

	vxbDevRelease(dev);
}
