/*

Dummy power domain driver to test the power domain API

DTS:

dummy_power_domain: dummy_power_domain@0 {
    compatible = "dummy_power_domain";
    #power-domain-cells = <0>;
};

dummy_power_domain_test {
    compatible = "dummy_power_domain_test";
    power-domains = <&dummy_power_domain>;
    power-domain-names = "domain_expansion";
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/util/vxbParamLib.h>
#include <subsys/power/vxbPdLib.h>

typedef struct {
	VXB_DEV_ID dev;
	VXB_PD_PROVIDER_ID provider;
	VXB_PD_ID pd;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dpdfdt[] = {
	{ "dummy_power_domain", NULL },
	{ NULL },
};

LOCAL STATUS
pdtranslate(VXB_DEV_ID dev, PD_TRANS_ARGS *trans, void *data, VXB_PD_ID *pd)
{
	printf("power domain translate: dev %p trans %p data %p pd %p\n",
	    dev, trans, data, pd);
	return OK;
}

LOCAL STATUS
pdon(VXB_PD_ID pd)
{
	printf("power domain on: %p\n", pd);
	return OK;
}

LOCAL STATUS
pdoff(VXB_PD_ID pd)
{
	printf("power domain off: %p\n", pd);
	return OK;
}

LOCAL void
dpdfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbPdUnregister(ctlr->provider);
	vxbPdDestroy(ctlr->pd);
	vxbMemFree(ctlr);
}

LOCAL int
dpdprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dpdfdt, NULL);
}

LOCAL STATUS
dpdattach(VXB_DEV_ID dev)
{
	static VXB_PD_FUNC pdfunc = {
		.powerOn = pdon,
		.powerOff = pdoff,
	};

	VXB_FDT_DEV *fdtdev;
	Ctlr *ctlr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	fdtdev = vxbFdtDevGet(dev);
	if (!ctlr || !fdtdev)
		goto error;

	ctlr->dev = dev;
	vxbDevSoftcSet(dev, ctlr);

	ctlr->provider = vxbPdRegister(dev, pdtranslate, NULL);
	if (!ctlr->provider)
		goto error;

	if (vxbPdCreate(&ctlr->pd, &pdfunc) != OK)
		goto error;

	vxbPdDataSet(ctlr->pd, ctlr);

	return OK;

error:
	dpdfree(ctlr);
	return ERROR;
}

LOCAL STATUS
dpddetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	dpdfree(ctlr);
	return OK;
}

LOCAL STATUS
dpdshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD dpddev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dpdprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dpdattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), dpddetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), dpdshutdown },
	VXB_DEVMETHOD_END,
};

LOCAL VXB_PARAMS dpdparams[] = {
	{ "param0", VXB_PARAM_INT32, { (void *)0 } },
	{ "param1", VXB_PARAM_INT64, { (void *)1 } },
	{ "param2", VXB_PARAM_STRING, { "test" } },
	{ NULL, VXB_PARAM_END_OF_LIST, { NULL } },
};

VXB_DRV dpddrv = {
	{ NULL },
	"dummy_power_domain",
	"Dummy power domain driver",
	VXB_BUSID_FDT,
	0,
	0,
	dpddev,
	dpdparams,
};

VXB_DRV_DEF(dpddrv)

STATUS
dpddrvadd(void)
{
	return vxbDrvAdd(&dpddrv);
}

void
dpddrvtest(void)
{
	VXB_DEV_ID dev;
	VXB_PD_ID pd;

	dev = vxbDevAcquireByName("dummy_power_domain_test", 0);
	if (dev == NULL) {
		printf("Failed to get power domain device\n");
		return;
	}

	if (vxbPdGetByIndex(dev, 0, &pd) == OK) {
		printf("PD by index %p\n", pd);
		vxbPdOn(pd);
		vxbPdOff(pd);
	}

	if (vxbPdGetByName(dev, "domain_expansion", &pd) == OK) {
		printf("PD by name %p\n", pd);
		vxbPdOn(pd);
		vxbPdOff(pd);
	}

	vxbPdOnAll(dev);
	vxbPdOffAll(dev);

	vxbDevRelease(dev);
}
