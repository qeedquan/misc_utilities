/*

Dummy PCI Controller

The portable PCI code in VxWorks uses the PCI controller to setup various PCI device configuration (For legacy setup, not MSIs)
It reads the PCI configuration header for the information on setting up the various resources (vxbResourceAlloc):
- BAR addresses
- INT_PIN to see if there is a interrupt setup; if there is, it also programs INT_LINE

There are various PCI drivers that match based on the class codes as opposed to the VID/DID,
these include the USB controllers (OHCI/UHCI/EHCI/XHCI), though the matching for them in VxWorks is limited and can skip valid PCI USB controller devices.

DTS:

The DTS specification for PCI is based on Linux but with some differences:
https://michael2012z.medium.com/understanding-pci-node-in-fdt-769a894a13cc

dummy_pci: dummy_pci@0 {
    compatible = "dummy_pci";
    device_type = "pci";

    // Size of various cells
    #address-cells = <2>;
    #size-cells = <2>;
    #interrupt-cells = <2>;

    // Interrupt parent handle
    // This interrupt controller will handle interrupts for the PCI controller
    // The PCI controller needs to implement a method for vxbPciIntAssign for interrupt assignments to work with the portable PCI code in VxWorks
    interrupt-parent = <&pic>;

    // PCI controller registers
    reg = <0x80000 0x100>;

    // ranges and interrupt-map can be optional if the firmware has set all of this up for us.

    // Used for address translation between a child node and its parent node, in our case it’s between PCI address space and the CPU address space.
    // The reason to do the translation is: Under a PCI bridge, devices can have their own address space.
    // If CPU need to access the device address space, a map has to be created.
    // The map matches the addresses in CPU’s view to the addresses in PCI space.
    // Only the size in PCI space is needed. The size in CPU space is not needed, because it has to be the same as that in the PCI space.
    // The values are determined by the #address-cells and #size-cells of parent or defined here
    // PCI address, CPU address, PCI size
    ranges = <0x0 0x1000 0x10000>;

    // The interrupt information is used by vxbFdtPciIntrGet()
    // The function vxbFdtPciIntrInfo() reads the FDT to get the interrupt information to store inside the struct fdtPciIntr
    // On platforms like x86, it will read from the ACPI/MP tables for the information instead of FDT (the vxbPciIntAssign will be assigned a different function)
    // For VxWorks, the number of cells that interrupt-map-mask and interrupt-map has is determined by the #address-cells + #interrupt-cells
    // Each entry in the interrupt-map needs to have a corresponding interrupt-map-mask entry due to the logic described below

    // The logic for matching the interrupt-map entries to devices is as follows:
    // The childAddr field is [pack_bdf(dev, bus, func), 0, 0, irq_pin]
    // if (childAddr[i] & interrupt-map-mask[i]) == interrupt-map[i]) { matched }
    // If the interrupt vectors are assigned wrong, there is a chance for the board to hang (possibly due to interrupt storms) when the device gets used

    // The interrupt-map-mask field contains 2 masks in 4 cells:
    // The first 3 cells is a mask for PCI address.
    // The last 1 cell is a mask for Pin.
    // Format: | Address mask |  | Pin mask|
    interrupt-map-mask = <0xffffff 0 0 7>;

    // Each line of the interrupt-map consists of 4 parts: PCI address + Pin + Interrupt Controller Handle + Interrupt Controller Data.
    interrupt-map = <
        0x00 0x00 0x00 0x01 0x8001 0x00 0x00 0x00 0x03 0x04
        0x00 0x00 0x00 0x01 0x8001 0x00 0x00 0x00 0x03 0x04
    >;

    // VxWorks uses this configuration format (above is Linux)
    // childAddr childInt &interrupt-parent intParAddr intParInt
    interrupt-map = <
        0x1000 0x0 0x0 0x1 &pic 0x21
    >;

    // bus-range contains two cells. The first one specifies the bus number of current PCI node, which is 0 here.
    // The other one specifies how many sub-level PCI buses can be supported by this PCI node, which is 0xff.
    // If this is not set correctly (the start bus number in particular), it can lead to devices skipped during a scan so it won't show up the PCI tree later.
    // The bootloader/firmware sets the bus number for the PCI controller and this entry is here to match it.
    bus-range = <0x00 0xff>;

    // Ignore initializing pci devices that match these values
    // The value of each entry is a <packed_value> = pack_bdf(bus, dev, func)
    vxworks,exclude-map = <0x1000 0x2000>;
    // the mask value that is applied to the packed bdf value before matching against the above entries in the exclude map
    vxworks,exclude-mask = <0xffffffff>;

    // Specify the device is not to be initialized during the first pass of scanning, but at a later pass
    // The values of the entries is the same as the exclude-map and exclude-mask values
    vxworks,defer-map = <0x1000 0x2000>;
    vxworks,defer-mask = <0xffffffff>;
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <sysLib.h>
#include <ioLib.h>
#include <iosLib.h>
#include <endian.h>
#include <subsys/int/vxbIntLib.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/pciDefines.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <hwif/buslib/pciDefines.h>
#include <hwif/buslib/vxbPciLib.h>
#include <hwif/buslib/vxbFdtPci.h>

typedef struct {
	// must be first field
	PCI_ROOT_CHAIN rootchain;

	struct fdtPciIntr intr;

	VXB_DEV_ID dev;
} Ctlr;

LOCAL VXB_FDT_DEV_MATCH_ENTRY dpcimatch[] = {
	{ "dummy_pci", NULL },
	{},
};

LOCAL void
dpcifree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr);
}

LOCAL STATUS
dpciprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dpcimatch, NULL);
}

LOCAL STATUS
dpciattach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;
	VXB_FDT_DEV *fdtdev;

	if (dev == NULL)
		return ERROR;

	fdtdev = vxbFdtDevGet(dev);
	if (fdtdev == NULL)
		return ERROR;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;

	if (vxbFdtPciIntrInfo(fdtdev, &ctlr->intr) != OK)
		goto error;

	vxbDevSoftcSet(dev, ctlr);

	return OK;

error:
	dpcifree(ctlr);
	return ERROR;
}

LOCAL STATUS
dpcicfgread(VXB_DEV_ID dev, PCI_HARDWARE *pcidev, UINT32 off, UINT32 width, void *data)
{
	return OK;
}

LOCAL STATUS
dpcicfgwrite(VXB_DEV_ID dev, PCI_HARDWARE *pcidev, UINT32 off, UINT32 width, UINT32 data)
{
	return OK;
}

LOCAL STATUS
dpciintassign(VXB_DEV_ID dev, PCI_HARDWARE *pcidev, int pin, UINT8 *interrupt, VXB_INTR_ENTRY *intrentry)
{
	Ctlr *ctlr;

	if (dev == NULL || pcidev == NULL || intrentry == NULL)
		return ERROR;

	ctlr = vxbDevSoftcGet(dev);
	if (ctlr == NULL)
		return ERROR;

	return vxbFdtPciIntrGet(&ctlr->intr, pcidev->pciBus, pcidev->pciDev, pcidev->pciFunc, pin, interrupt, intrentry);
}

LOCAL VXB_RESOURCE *
dpciresalloc(VXB_DEV_ID dev, VXB_DEV_ID child, UINT32 id)
{
	VXB_RESOURCE *res;
	VXB_RESOURCE_ADR *resadr;
	PCI_HARDWARE *pcidev;

	if (child == NULL)
		return NULL;

	pcidev = vxbDevIvarsGet(child);
	res = vxbResourceFind(&pcidev->vxbResList, id);
	if (res == NULL)
		return NULL;

	switch (VXB_RES_TYPE(res->id)) {
	case VXB_RES_MEMORY:
	case VXB_RES_IO:
		resadr = res->pRes;
		if (!resadr->virtual && vxbRegMap(res) != OK)
			return NULL;
		break;

	case VXB_RES_IRQ:
		if (vxbIntMap(res) != OK)
			return NULL;
		break;

	default:
		return NULL;
	}

	return res;
}

LOCAL STATUS
dpciresfree(VXB_DEV_ID dev, VXB_DEV_ID child, VXB_RESOURCE *res)
{
	int restype;

	if (child == NULL)
		return ERROR;

	restype = VXB_RES_TYPE(res->id);
	if (restype == VXB_RES_MEMORY || restype == VXB_RES_IO)
		return vxbRegUnmap(res);

	return OK;
}

LOCAL VXB_RESOURCE_LIST *
dpcireslistget(VXB_DEV_ID dev, VXB_DEV_ID child)
{
	PCI_HARDWARE *pcidev;

	if (dev == NULL)
		return NULL;

	pcidev = vxbDevIvarsGet(child);
	if (pcidev == NULL)
		return NULL;

	return &pcidev->vxbResList;
}

LOCAL VXB_DRV_METHOD dpcimethods[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dpciprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dpciattach },
	{ VXB_DEVMETHOD_CALL(vxbDevIoctl), vxbPciBusIoctl },
	{ VXB_DEVMETHOD_CALL(vxbPciCfgRead), dpcicfgread },
	{ VXB_DEVMETHOD_CALL(vxbPciCfgWrite), dpcicfgwrite },
	{ VXB_DEVMETHOD_CALL(vxbPciIntAssign), dpciintassign },
	{ VXB_DEVMETHOD_CALL(vxbResourceFree), dpciresfree },
	{ VXB_DEVMETHOD_CALL(vxbResourceAlloc), (FUNCPTR)dpciresalloc },
	{ VXB_DEVMETHOD_CALL(vxbResourceListGet), (FUNCPTR)dpcireslistget },
	VXB_DEVMETHOD_END,
};

VXB_DRV dpcidrv = {
	{ NULL },
	"dummy_pci",
	"Dummy PCI Controller Driver",
	VXB_BUSID_FDT,
	0,
	0,
	dpcimethods,
	NULL,
};

VXB_DRV_DEF(dpcidrv)

void
dpcilink(void)
{
}
