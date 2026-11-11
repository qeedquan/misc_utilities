/*

The PCI bus scan code tries to enable all the devices in configuration space and let the drivers handle the initialization of the device itself.
For some devices, enabling them in configuration space will cause them to trigger endless interrupts if not acknowledged, leading to hangs.
This driver disables the devices in the match list, effectively skipping the device initialization for it (by undoing what the PCI scanning initialization does).

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <ioLib.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbPciLib.h>
#include <hwif/buslib/pciDefines.h>

LOCAL const VXB_PCI_DEV_MATCH_ENTRY pciskippci[] = {
	{},
};

LOCAL STATUS
pciskipprobe(VXB_DEV_ID dev)
{
	return vxbPciDevMatch(dev, pciskippci, NULL);
}

LOCAL STATUS
pciskipattach(VXB_DEV_ID dev)
{
	UINT16 cmd;

	// disable the device
	VXB_PCI_BUS_CFG_READ(dev, PCI_CFG_COMMAND, 2, cmd);
	cmd &= ~(PCI_CMD_IO_ENABLE | PCI_CMD_MEM_ENABLE | PCI_CMD_MASTER_ENABLE);
	cmd |= PCI_CMD_INTX_DISABLE;
	VXB_PCI_BUS_CFG_WRITE(dev, PCI_CFG_COMMAND, 2, cmd);

	return OK;
}

LOCAL VXB_DRV_METHOD pciskipdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), pciskipprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), pciskipattach },
	VXB_DEVMETHOD_END,
};

VXB_DRV pciskipdrv = {
	{ NULL },
	"pci-skip",
	"PCI Skip",
	VXB_BUSID_PCI,
	0,
	0,
	pciskipdev,
	NULL,
};

VXB_DRV_DEF(pciskipdrv)

void
pciskiplink(void)
{
}
