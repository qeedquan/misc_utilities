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
    // 1 cell usually define <irq>
    #interrupt-cells = <2>;
};

// a device that uses the dummy_intctrl
dummy_intctrl_test@0 {
    compatible = "dummy_intctrl_test";

    // it is possible to specify multiple interrupts of the same vector, they are linked together in that case
    // the interrupts with the same vector can be registered a handler using vxbIntConnect like any other vectors
    // all of the handlers registered will get executed in order of registration when that particular interrupt triggers
    interrupts = <45 0
                  45 0
                  84 1
                  53 2>;

    interrupt-parent = <&dummy_intctrl>;
};

Minimally, a functional interrupt controller should implement at least IntEnable/IntDisable.
There can be multiple interrupt controllers in the system, and one interrupt controller can be a parent of another one.

The command isrShow, however, will show all the interrupt handlers registered but does not show which parent the handlers belong to.
The interrupt controllers do not store the handlers in it's structure, rather the handlers/args are stored in a global array.
That array can be indexed by a tag field described by isrShow.
The tag field in the isrShow is determined by the following method:

Let say there are two interrupt controllers A and B.
A defines maxirqs to be 50
B defines maxirqs to be 128

The tag field for A will be values from 1 to 50.
The tag field for B will be values from 51 to 178 (Since the base will start at 50, assume interrupts numbering is 1-based here).
So, according to the numbering scheme above, a interrupt will have the following value:
interrupt_int_base + interrupt_number.
If the dummy_intctrl is interrupt controller B, then the tag fields for the interrupts will be:
95 (45 + 50)
103 (53 + 50)
134 (84 + 50)

The tag field number can be used with VXB_INT_ISR_CALL to call the interrupt handler directly

An alternative command is vxbIntShow, which shows alot more details about the ISR handlers and which drivers they belong to

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
	VXB_RESOURCE *intres[4];
	UINT32 intbase;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dintcfdt[] = {
	{ "dummy_intctrl", NULL },
	{ NULL },
};

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dintctestfdt[] = {
	{ "dummy_intctrl_test", NULL },
	{ NULL },
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
dintctestint1(void *ctx)
{
	kprintf("%s: %p\n", __func__, ctx);
}

LOCAL void
dintctestint2(void *ctx)
{
	kprintf("%s: %p\n", __func__, ctx);
}

LOCAL void
dintctestint3(void *ctx)
{
	kprintf("%s: %p\n", __func__, ctx);
}

LOCAL void
dintctestint4(void *ctx)
{
	kprintf("%s: %p\n", __func__, ctx);
}

LOCAL int
dintctestprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dintctestfdt, NULL);
}

LOCAL STATUS
dintctestattach(VXB_DEV_ID dev)
{
	static const VOIDFUNCPTR handlers[] = { dintctestint1, dintctestint2, dintctestint3, dintctestint4 };

	Ctlr *ctlr;
	UINT16 i;

	kprintf("intctrl test: attach %p\n", dev);
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
		vxbIntConnect(ctlr->dev, ctlr->intres[i], handlers[i % NELEMENTS(handlers)], ctlr);
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

	vxbDevSoftcSet(dev, ctlr);

	kprintf("intctrl: %p attached: base %" PRId32 "\n", dev, ctlr->intbase);
	return OK;

error:
	kprintf("intctrl: failed to attached\n");
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
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dintcprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dintcattach },

	{ VXB_DEVMETHOD_CALL(vxbIntConfig), dintcconfig },
	{ VXB_DEVMETHOD_CALL(vxbIntEnable), dintcenable },
	{ VXB_DEVMETHOD_CALL(vxbIntDisable), dintcdisable },
	{ VXB_DEVMETHOD_CALL(vxbIntBind), dintcbind },
	{ VXB_DEVMETHOD_CALL(vxbIntPropSet), dintcpropset },
	{ VXB_DEVMETHOD_CALL(vxbIntIpi), dintcipi },

	VXB_DEVMETHOD_END,
};

LOCAL VXB_DRV_METHOD dintctestdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dintctestprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dintctestattach },

	VXB_DEVMETHOD_END,
};

VXB_DRV dintcdrv = {
	{ NULL },
	"dummy_intctrl",
	"Dummy interrupt controller driver",
	VXB_BUSID_FDT,
	0,
	0,
	dintcdev,
	NULL,
};

VXB_DRV dintctestdrv = {
	{ NULL },
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

void
dintclink(void)
{
}

void
dintctest(void)
{
	VXB_DEV_ID dev;
	Ctlr *ctlr;

	dev = vxbDevAcquireByName("dummy_intctrl", 0);
	if (!dev) {
		printf("Failed to get interrupt controller\n");
		return;
	}

	ctlr = vxbDevSoftcGet(dev);
	printf("Interrupt Device: %p Controller: %p\n", dev, ctlr);
	printf("Base: %" PRId32 "\n", ctlr->intbase);

	// This is used by the interrupt controller to call the interrupt handler registered
	VXB_INT_ISR_CALL(ctlr->intbase + 45);
	VXB_INT_ISR_CALL(ctlr->intbase + 53);
	VXB_INT_ISR_CALL(ctlr->intbase + 84);

	// We can call an invalid one and it should not crash (it should call a stray handler)
	VXB_INT_ISR_CALL(ctlr->intbase + 8499);

	vxbDevRelease(dev);
}
