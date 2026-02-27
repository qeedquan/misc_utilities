#ifndef _VXB_PCI_LIB_H_
#define _VXB_PCI_LIB_H_

typedef struct {
	UINT8 pciBus;
	UINT8 pciDev;
	UINT8 pciFunc;
	UINT8 pciIntPin;
	UINT16 pciVendId;
	UINT16 pciDevId;
} PCI_HARDWARE;

#define VXB_BUSID_PCI "pci"

int vxbPciConfigBdfPack(int bus, int device, int func);

#endif
