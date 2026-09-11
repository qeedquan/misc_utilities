/*

Dummy clock driver to test the clock API

DTS:

dummy-clk {
    compatible = "dummy-clk";

    // specifies the cell number of clock registered to clock subsystem in this driver
    // if there is one clock that can be enable/disabled, specify <0>
    // if there are more than one clocks, specify <1>
    // other device drivers will specify an id for subclock to be enabled
    #clock-cells = <0>;

    // default frequency of the clock
    clock-frequency = <123456789>;

    // set the clock name, if this doesn't exist it uses the name of the device tree node
    clock-output-names = "superfakeclk";
};

Other device drivers can refer to clocks as so:

device {
    clocks: <&dummy-clk>

    // if clock-cells is <1>, specify the clock id here
    // clocks: <&dummy-clk 50>

    // An alternative to specifying a clock
    // This creates a fake clock device in VxWorks with that frequency so the device can refer to it
    clock-frequency = <987654321>;
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <subsys/clk/vxbClkLib.h>

typedef struct {
	VXB_CLK_REG clkreg;
	VXB_CLK_DOMAIN_REG clkdm;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dclkfdt[] = {
	{ "dummy-clk", NULL },
	{ NULL },
};

LOCAL STATUS
clkenable(VXB_CLK *clk)
{
	printf("clk enable: %p\n", clk);
	return OK;
}

LOCAL STATUS
clkdisable(VXB_CLK *clk)
{
	printf("clk disable: %p\n", clk);
	return OK;
}

LOCAL STATUS
clkrateset(VXB_CLK *clk, UINT64 rate)
{
	printf("clk rate set: %p %" PRIu64 "\n", clk, rate);
	return OK;
}

LOCAL UINT64
clkrateget(VXB_CLK *clk, UINT64 parentrate)
{
	printf("clk rate get: %p %" PRIu64 "\n", clk, parentrate);
	return parentrate;
}

LOCAL UINT32
clkstatusget(VXB_CLK *clk)
{
	printf("clk status get: %p\n", clk);
	return CLOCK_STATUS_ENABLED;
}

LOCAL STATUS
clkparentset(VXB_CLK *clk, VXB_CLK *parent)
{
	printf("clk parent set: %p %p\n", clk, parent);
	return OK;
}

LOCAL STATUS
clkinit(VXB_CLK *clk)
{
	printf("clk init: %p\n", clk);
	return OK;
}

LOCAL STATUS
clkextctrl(VXB_CLK *clk, UINT32 ctrl, void *ctx)
{
	printf("clk parent set: %p %" PRIu32 " %p\n", clk, ctrl, ctx);
	return OK;
}

LOCAL void
dclkfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr);
}

LOCAL int
dclkprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dclkfdt, NULL);
}

LOCAL STATUS
dclkattach(VXB_DEV_ID dev)
{
	static VXB_CLK_FUNC clkops = {
		clkenable,
		clkdisable,
		clkrateset,
		clkrateget,
		clkstatusget,
		clkparentset,
		clkinit,
		clkextctrl,
	};

	VXB_FDT_DEV *fdtdev;
	VXB_CLK_DOMAIN_REG *clkdm;
	VXB_CLK_REG *clkreg;
	VXB_CLK *clk;
	Ctlr *ctlr;
	const UINT32 *prop;
	int fdtoff, len;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	fdtdev = vxbFdtDevGet(dev);
	if (!ctlr || !fdtdev)
		goto error;

	vxbDevSoftcSet(dev, ctlr);
	fdtoff = fdtdev->offset;

	clkdm = &ctlr->clkdm;
	clkdm->name = "dummy-clkdm";

	clkreg = &ctlr->clkreg;
	clkreg->pDev = dev;
	clkreg->name = (char *)vxFdtPropGet(fdtoff, "clock-output-names", &len);
	if (!clkreg->name)
		clkreg->name = (char *)vxFdtGetName(fdtoff, NULL);
	clkreg->domainName = clkdm->name;
	clkreg->pFuncs = &clkops;
	clkreg->pClkData = NULL;
	clkreg->clkType = VXB_CLK_FAKE;
	clkreg->offset = fdtoff;

	if (vxbClkDomainRegister(clkdm) == NULL)
		goto error;

	clk = vxbClkRegister(clkreg);
	clk->clkRate = 0;
	prop = vxFdtPropGet(fdtoff, "clock-frequency", &len);
	if (prop)
		clk->clkRate = vxFdt32ToCpu(*prop);

	return OK;

error:
	dclkfree(ctlr);
	return ERROR;
}

LOCAL STATUS
dclkdetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	dclkfree(ctlr);
	return OK;
}

LOCAL STATUS
dclkshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD dclkdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dclkprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dclkattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), dclkdetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), dclkshutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV dclkdrv = {
	{ NULL },
	"dummy-clk",
	"Dummy clock driver",
	VXB_BUSID_FDT,
	0,
	0,
	dclkdev,
	NULL,
};

VXB_DRV_DEF(dclkdrv)

STATUS
dclkdrvadd(void)
{
	return vxbDrvAdd(&dclkdrv);
}

void
dclkdrvtest(void)
{
	VXB_DEV_ID dev;
	VXB_CLK_ID clkid;

	dev = vxbDevAcquireByName("dummy-clk", 0);
	if (!dev) {
		printf("failed to get clock\n");
		return;
	}

	clkid = vxbClkGet(dev, "superfakeclk");
	printf("clock id: %p\n", clkid);
	printf("status: %#" PRIx32 "\n", vxbClkStatusGet(clkid));
	printf("rate: %" PRId64 "\n", vxbClkRateGet(clkid));
	vxbDevRelease(dev);
}
