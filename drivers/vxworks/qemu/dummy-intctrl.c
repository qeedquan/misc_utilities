/*

Dummy interrupt controller to test the API

DTS:

dummy_intctrl: dummy_intctrl@0 {
    interrupt-controller;

    compatible = "dummy_intctrl";

    // usually set to 0 since there is no cells for this
    #address-cells = <0>;

    // specifies the number of fields <interrupt> has in the child
    // standard cells are 2/4/6
    // irq          the irq number meaning is a unique value whose behavior is determined by the interrupt controller
    // level-sense  edge or level triggered (hi-lo or lo-hi transition, lo, hi)
    //
    // 2 cells usually defines <irq, level-sense>
    #interrupt-cells = <2>;
};

// a device that uses the dummy_intctrl
dummy_intctrl_test@0 {
    compatible = "dummy_intctrl_test";

    interrupts = <45 0
                  84 1
                  53 2>;

    interrupt-parent = <&dummy_intctrl>;
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <cpuset.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/vxbus/vxbLib.h>
#include <subsys/int/vxbIntLib.h>

typedef struct {
	VXB_DEV_ID dev;
	VXB_RESOURCE *intres[3];
	UINT32 intbase;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dintcfdt[] = {
    {"dummy_intctrl", NULL},
    {NULL},
};

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dintctestfdt[] = {
    {"dummy_intctrl_test", NULL},
    {NULL},
};

LOCAL void
dintcfree(Ctlr *ctlr)
{
	size_t i;

	if (!ctlr)
		return;

	for (i = 0; i < NELEMENTS(ctlr->intres); i++)
		vxbResourceFree(ctlr->dev, ctlr->intres[i]);

	vxbMemFree(ctlr);
}

LOCAL void
dintctestint(void *ctx)
{
	(void)ctx;
}

LOCAL int
dintctestprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dintctestfdt, NULL);
}

LOCAL STATUS
dintctestattach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;
	size_t i;

	printf("intctrl test: attach %p\n", dev);
	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;

	for (i = 0; i < NELEMENTS(ctlr->intres); i++) {
		ctlr->intres[i] = vxbResourceAlloc(dev, VXB_RES_IRQ, i);
		if (!ctlr->intres[i])
			goto error;
	}

	for (i = 0; i < NELEMENTS(ctlr->intres); i++) {
		vxbIntConnect(ctlr->dev, ctlr->intres[i], dintctestint, ctlr);
		vxbIntEnable(ctlr->dev, ctlr->intres[i]);
	}

	vxbDevSoftcSet(dev, ctlr);

	return OK;

error:
	printf("intctrl test: failed to attach\n");
	dintcfree(ctlr);
	return ERROR;
}

LOCAL int
dintcprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dintcfdt, NULL);
}

LOCAL STATUS
dintcattach(VXB_DEV_ID dev)
{
	static const int maxirq = 128;
	static const int maxipi = 0;

	Ctlr *ctlr;
	VXB_FDT_DEV *fdtdev;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;

	fdtdev = vxbDevIvarsGet(dev);
	if (!fdtdev)
		goto error;

	if (vxbIntRegister(dev, fdtdev->offset, maxirq, maxipi, &ctlr->intbase) != OK)
		goto error;

	printf("intctrl: %p attached: base %" PRIx32 "\n", dev, ctlr->intbase);
	return OK;

error:
	printf("intctrl: failed to attached\n");
	dintcfree(ctlr);
	return ERROR;
}

LOCAL STATUS
dintcconfig(VXB_DEV_ID dev, UINT32 vector, VXB_INTR_ENTRY *entry)
{
	size_t i;

	kprintf("intctrl configure: %p vector=%" PRId32 " entry=%p node %" PRIx32 " [ ", dev, vector, entry, entry->node);
	for (i = 0; i < entry->numProp; i++)
		kprintf("%" PRId32 " ", entry->pProp[i]);
	kprintf("]\n");
	return OK;
}

LOCAL STATUS
dintcenable(VXB_DEV_ID dev, UINT32 vector, VXB_INTR_ENTRY *entry)
{
	kprintf("intctrl enable: %p vector=%" PRId32 " entry=%p\n", dev, vector, entry);
	return OK;
}

LOCAL STATUS
dintcdisable(VXB_DEV_ID dev, UINT32 vector, VXB_INTR_ENTRY *entry)
{
	kprintf("intctrl disable: %p vector=%" PRId32 " entry=%p\n", dev, vector, entry);
	return OK;
}

LOCAL STATUS
dintcbind(VXB_DEV_ID dev, UINT32 vector, cpuset_t dest, VXB_INTR_ENTRY *entry)
{
	kprintf("intctrl bind: %p vector=%" PRId32 " dest=%#x entry=%p\n", dev, vector, dest, entry);
	return OK;
}

LOCAL STATUS
dintcpropset(VXB_DEV_ID dev, UINT32 vector, VXB_INTR_ENTRY *entry)
{
	kprintf("intctrl bind: %p vector=%" PRId32 " dest=%#x entry=%p\n", dev, vector, entry);
	return OK;
}

LOCAL STATUS
dintcipi(VXB_DEV_ID dev, UINT32 vector, phys_cpuset_t physcpus)
{
	kprintf("intctrl ipi: %p vector=%" PRId32 " physcpus=%#x\n", dev, vector, physcpus);
	return OK;
}

LOCAL VXB_DRV_METHOD dintcdev[] = {
    {VXB_DEVMETHOD_CALL(vxbDevProbe), dintcprobe},
    {VXB_DEVMETHOD_CALL(vxbDevAttach), dintcattach},

    {VXB_DEVMETHOD_CALL(vxbIntConfig), dintcconfig},
    {VXB_DEVMETHOD_CALL(vxbIntEnable), dintcenable},
    {VXB_DEVMETHOD_CALL(vxbIntDisable), dintcdisable},
    {VXB_DEVMETHOD_CALL(vxbIntBind), dintcbind},
    {VXB_DEVMETHOD_CALL(vxbIntPropSet), dintcpropset},
    {VXB_DEVMETHOD_CALL(vxbIntIpi), dintcipi},

    VXB_DEVMETHOD_END,
};

LOCAL VXB_DRV_METHOD dintctestdev[] = {
    {VXB_DEVMETHOD_CALL(vxbDevProbe), dintctestprobe},
    {VXB_DEVMETHOD_CALL(vxbDevAttach), dintctestattach},

    VXB_DEVMETHOD_END,
};

VXB_DRV dintcdrv = {
    {NULL},
    "dummy_intctrl",
    "Dummy interrupt controller driver",
    VXB_BUSID_FDT,
    0,
    0,
    dintcdev,
    NULL,
};

VXB_DRV dintctestdrv = {
    {NULL},
    "dummy_intctrl_test",
    "Dummy interrupt controller test driver",
    VXB_BUSID_FDT,
    0,
    0,
    dintctestdev,
    NULL,
};

VXB_DRV_DEF(dintcdrv)
VXB_DRV_DEF(dintctestdrv)

STATUS
dintcdrvadd(void)
{
	STATUS status;

	status = vxbDrvAdd(&dintcdrv);
	status |= vxbDrvAdd(&dintctestdrv);
	return status;
}
