/*

Dummy Regulator driver to test the Regulator API

DTS:

dummy_regulator: dummy_regulator {
    compatible = "dummy_regulator";
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <inttypes.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <subsys/regulator/vxbRegulator.h>

typedef struct {
	VXB_DEV_ID dev;
	REGULATOR regulator;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dregulatorfdt[] = {
	{ "dummy_regulator", NULL },
	{ NULL },
};

LOCAL STATUS
voltageset(REGULATOR *regulator, UINT32 microvolt)
{
	printf("%s(regulator=%p, microvolt=%" PRIu32 ")\n", __func__, regulator, microvolt);
	return OK;
}

LOCAL STATUS
dregulatorget(VXB_DEV_ID dev, REGULATOR **regulator)
{
	Ctlr *ctlr;

	printf("%s(dev=%p, regulator=%p)\n", __func__, dev, regulator);

	ctlr = vxbDevSoftcGet(dev);
	*regulator = &ctlr->regulator;
	return OK;
}

LOCAL void
dregulatorfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr);
}

LOCAL int
dregulatorprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dregulatorfdt, NULL);
}

LOCAL STATUS
dregulatorattach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	ctlr->regulator.pDev = dev;
	ctlr->regulator.pVoltageSet = voltageset;

	vxbDevSoftcSet(dev, ctlr);

	return OK;

error:
	dregulatorfree(ctlr);
	return ERROR;
}

LOCAL VXB_DRV_METHOD dregulatordev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dregulatorprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dregulatorattach },

	{ VXB_DEVMETHOD_CALL(vxbRegulatorGet), (FUNCPTR)dregulatorget },

	VXB_DEVMETHOD_END,
};

VXB_DRV dregulatordrv = {
	{ NULL },
	"dummy_regulator",
	"Dummy Regulator driver",
	VXB_BUSID_FDT,
	0,
	0,
	dregulatordev,
	NULL,
};

VXB_DRV_DEF(dregulatordrv)

STATUS
dregulatordrvadd(void)
{
	return vxbDrvAdd(&dregulatordrv);
}

void
dregulatordrvtest(void)
{
	VXB_DEV_ID dev;
	REGULATOR *regulator;

	dev = vxbDevAcquireByName("dummy_regulator", 0);
	if (!dev) {
		printf("Failed to get device\n");
		return;
	}

	VXB_REGULATOR_GET(dev, &regulator);
	if (!regulator)
		printf("Failed to get regulator\n");
	else
		vxbRegulatorVoltageSet(regulator, 2345);

	vxbDevRelease(dev);
}
