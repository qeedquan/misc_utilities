/*

Dummy pinmux driver to test the pinmux API
Another common name for pinmux is pad configuration

DTS:

dummy_pinmux: dummy_pinmux@0 {
    compatible = "dummy-pinmux";

    // this is mandatory as it points to the first one used by enable
    pinmux-0 = <&default>;

    // these are optional but the if the API needs the calls on the id, it needs it to be defined
    // pinmux enables the entries in the list by looping over the list and calling enable for each entry
    pinmux-1 = <&alt0 &alt1>;
    pinmux-2 = <&default &alt1>;

    // optional, this is used to get id mappings from the names
    pinmux-names = "default", "alt0", "alt1";

    // pin-set is not standardized, each pinmux controller put different values for their internal use
    // for this pinmux, it encodes address and values
    default: default {
        pin-set = <0x33 0x44
                   0x55 0x88>;
    };

    alt0: alt0 {
        pin-set = <0x5524 0x341>;
    };

    alt1: alt1 {
        pin-set = <0x34 0x52
                   0x24 1431>;
    };
};

Other drivers that need to mux pin will reference it like so:

dummy_pinmux_test {
    compatible = "dummy-pinmux-test";
    pinmux-0 = <&alt0>;
    pinmux-1 = <&alt1>;
};

The drivers will call vxbPinMuxEnable() to enable the pin-set described

In conjunction with other settings of the system, wrong pinmux info can cause the system to crash

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <subsys/pinmux/vxbPinMuxLib.h>

typedef struct {
	VXB_PINMUX_CTRL pinmux;
	VXB_DEV_ID dev;

	int initted;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dpmfdt[] = {
	{ "dummy-pinmux", NULL },
	{ NULL },
};

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dpmtestfdt[] = {
	{ "dummy-pinmux-test", NULL },
	{ NULL },
};

LOCAL void
dpmfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	if (ctlr->initted)
		vxbPinMuxUnregister(&ctlr->pinmux);

	vxbMemFree(ctlr);
}

LOCAL int
dpmprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dpmfdt, NULL);
}

LOCAL STATUS
dpmattach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	vxbDevSoftcSet(dev, ctlr);

	ctlr->pinmux.pDev = dev;
	if (vxbPinMuxRegister(&ctlr->pinmux) != OK)
		goto error;
	ctlr->initted = 1;

	if (vxbPinMuxEnable(ctlr->dev) != OK)
		goto error;

	return OK;

error:
	dpmfree(ctlr);
	return ERROR;
}

LOCAL STATUS
dpmdetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	dpmfree(ctlr);
	return OK;
}

LOCAL STATUS
dpmshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL STATUS
dpmenable(VXB_DEV_ID dev, INT32 offset)
{
	static const int entrylen = 2 * sizeof(UINT32);

	const UINT32 *pins;
	const void *name;
	int namelen, len;
	UINT32 addr, val;

	name = vxFdtGetName(offset, &namelen);
	pins = vxFdtPropGet(offset, "pin-set", &len);
	if (!pins || !name) {
		printf("failed to get name or pin-set\n");
		return ERROR;
	}

	printf("enabling mux '%s' (length: %d)\n", name, len);
	for (; len - entrylen >= 0; len -= entrylen) {
		addr = vxFdt32ToCpu(*pins++);
		val = vxFdt32ToCpu(*pins++);
		printf("%#x %#x\n", addr, val);
	}
	printf("\n");

	return OK;

	(void)dev;
}

LOCAL STATUS
dpmdisable(VXB_DEV_ID dev, INT32 offset)
{
	const void *name;
	int namelen;

	name = vxFdtGetName(offset, &namelen);
	if (!name)
		return ERROR;

	printf("disabling mux %s\n", name);
	return OK;

	(void)dev;
}

LOCAL STATUS
dpmshow(VXB_DEV_ID dev, INT32 verbose)
{
	printf("dummy-pinmux show\n");
	return OK;

	(void)dev;
	(void)verbose;
}

LOCAL int
dpmtestprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dpmtestfdt, NULL);
}

LOCAL STATUS
dpmtestattach(VXB_DEV_ID dev)
{
	// pinmux using the default id
	vxbPinMuxEnable(dev);

	// specify which pinmux id we want to enable
	vxbPinMuxEnableById(dev, 0);
	vxbPinMuxEnableById(dev, 1);

	return OK;
}

LOCAL VXB_DRV_METHOD dpmdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dpmprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dpmattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), dpmdetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), dpmshutdown },

	{ VXB_DEVMETHOD_CALL(vxbPinMuxEnable), dpmenable },
	{ VXB_DEVMETHOD_CALL(vxbPinMuxDisable), dpmdisable },
	{ VXB_DEVMETHOD_CALL(vxbPinMuxShow), dpmshow },
	VXB_DEVMETHOD_END,
};

LOCAL VXB_DRV_METHOD dpmtestdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dpmtestprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dpmtestattach },
	VXB_DEVMETHOD_END,
};

VXB_DRV dpmdrv = {
	{ NULL },
	"dummy-pinmux",
	"Dummy pinmux driver",
	VXB_BUSID_FDT,
	0,
	0,
	dpmdev,
	NULL,
};

VXB_DRV dpmtestdrv = {
	{ NULL },
	"dummy-pinmux-test",
	"Dummy pinmux test driver",
	VXB_BUSID_FDT,
	0,
	0,
	dpmtestdev,
	NULL,
};

VXB_DRV_DEF(dpmdrv)
VXB_DRV_DEF(dpmtestdrv)

STATUS
dpmdrvadd(void)
{
	STATUS status;

	status = vxbDrvAdd(&dpmdrv);
	status |= vxbDrvAdd(&dpmtestdrv);
	return status;
}

void
dpmdrvtest(void)
{
	static char *ids[] = { "default", "alt0", "alt1", NULL };

	VXB_DEV_ID dev;
	UINT32 id;
	size_t i;

	dev = vxbDevAcquireByName("dummy-pinmux", 0);
	if (dev == NULL) {
		printf("Failed to get pin mux device\n");
		return;
	}

	// need pinmux-names and pinmux-n to be set
	for (i = 0; ids[i]; i++) {
		id = vxbPinMuxGetIdByName(dev, ids[i]);
		printf("ID %s: %x\n", ids[i], id);
		vxbPinMuxEnableById(dev, id);
		vxbPinMuxDisableById(dev, id);
	}

	// enable/disable the default pinmux
	vxbPinMuxEnable(dev);
	vxbPinMuxDisable(dev);

	vxbDevRelease(dev);
}
