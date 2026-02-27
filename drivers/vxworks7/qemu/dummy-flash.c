/*

Dummy Flash driver to test the Flash API

DTS:

dummy_flash: dummy_flash@0 {
    compatible = "dummy_flash";
};

Flash devices are usually exposed through MMIO, where the interface to the registers and data is just memory addresses we can write to
Writing special bytes to a specific addresses will enable the flash chip to return separate information from the data (CFI, JEDEC, status, etc)
On startup, drivers will query the flash CFI/JEDEC IDs to figure out what kind of flash it is
Flash chips registered on the system can be shown with vxbFlashChipShow()

NOR Flash devices will set the bits to 1 on erase, TFFS expects this erase behavior

---

To register the flash chip for use with TFFS filesystem:

1. Add VIP option INCLUDE_MTD_VXBFLASH
2. Set VXBFLASH_CFG_STR $dummy_flash0#0:0x0,0x4000000,0x0,rfa0
   The general syntax is $flashInstanceName#index:startAddress,length,bootSize,partitionName

tffsDevCreate() can fail if length >= chip_size or an the startAddress is invalid on a real flash chip.

Once the flash chip is registered with the TFFS filesystem, we can format it with sysTffsFormat()
To show information about the TFFS filesystem, use tffsShowAll()

The VIP option INCLUDE_TFFS_MOUNT will try to automount the detected drives on startup.
For manual mounting:

To mount a TFFS drive:
usrTffsConfig 0, 0, "/tffs0" (calls tffsDevCreate/xbdBlkDevCreateSync underneath)

Alternative way
sysTffsFormat
dev = tffsDevCreate (0,0)
xbdBlkDevCreateSync dev, "/tffs0"
dosFsVolFormat "/tffs0:0"

sysTffsRead/sysTffsWrite is used with the TFFS filesystem, it will call the flash reads/writes underneath

The TFFS filesystem needs the base address of the flash chip, it uses this do raw header read/writes for block allocation
The base address of the chip has to be 4kb aligned, writes will be out of bounds if not aligned since the window page setting code will zero out the lower 12 bits of the address

This is stored in the 'private' field of the flash structure

The TFFS format routine will write a unit header every 64kb block

----

To register the flash chip as a MTD device (alternative to TFFS):
1. Add VIP option INCLUDE_MTD
2. Set MTD_CFG_STR "$dummy_flash0#0:0x0,0x800,/flash0#0,1"

The general syntax is:
flashInstanceName#chipIndex:\flashPartition0string;\flashPartition0string;\...
Each flash partition string consists of:
startBlockAddress,blockCount,mtdDeviceName#flashGroupIndex,flashUsage

flashUsage:
0 - FTL
1 - NVRAM

Use mtdShow(), mtdChipShow() to show information about the registered MTD device

Using MTD devices is usually for NAND devices for use in conjunction with the NFTL library (NFTL library only works with NAND devices)
Another usage is for using it as NVRAM (usrFlashNvRam)

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <subsys/flash/vxbFlashLib.h>
#include <vxbFlash.h>
#include <vxbFlashCommon.h>
#include <norflash/vxbNorFlash.h>

enum {
	CHIPSIZE = 64 * 1024 * 1024,
	SECTSIZE = 64 * 1024,
	ALIGN = 4096,
};

typedef struct {
	VXB_FLASHCTRL flash;
	FLASH_CHIP flashchip;
	VXB_DEV_ID dev;
	NOR_FLASH_INFO norinfo;
	UINT8 *alloc;
	UINT8 *mem;
} Ctlr;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dflashfdt[] = {
	{ "dummy_flash", NULL },
	{ NULL },
};

LOCAL void *
alignaddr(void *ptr, UINT64 align)
{
	uintptr_t addr;

	addr = (uintptr_t)ptr;
	addr = (addr + align - 1) & ~(align - 1);
	return (void *)addr;
}

LOCAL void
dflashfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr->alloc);
	vxbMemFree(ctlr);
}

LOCAL int
dflashprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dflashfdt, NULL);
}

LOCAL STATUS
dflashenable(FLASH_CHIP_ID flashchip, BOOL enable, void *op)
{
	printf("%s(flashchip=%p, enable=%d, op=%p)\n", __func__, flashchip, enable, op);
	return OK;
}

LOCAL STATUS
dflashreset(FLASH_CHIP_ID flashchip)
{
	printf("%s(flashchip=%p)\n", __func__, flashchip);
	return OK;
}

LOCAL STATUS
dflashread(FLASH_CHIP_ID flashchip, FLASH_ADDR_T addr, UINT32 bufcount, FLASH_SIZE_T buflen, UINT8 **buf, void *op)
{
	Ctlr *ctlr;
	UINT32 i;

	printf("%s(flashchip=%p, addr=%lx, bufcount=%d, buflen=%lu, buf=%p, op=%p)\n",
	    __func__, flashchip, (ULONG)addr, bufcount, (ULONG)buflen, buf, op);

	if (addr >= CHIPSIZE || addr + (bufcount * buflen) > CHIPSIZE)
		return ERROR;

	for (i = 0; i < bufcount; i++) {
		if (buf[i] == NULL)
			return ERROR;
	}

	ctlr = vxbDevSoftcGet(flashchip->pDev);
	for (i = 0; i < bufcount; i++) {
		memcpy(buf[i], ctlr->mem + addr + (buflen * i), buflen);
	}

	return OK;
}

LOCAL STATUS
dflashwrite(FLASH_CHIP_ID flashchip, FLASH_ADDR_T addr, UINT32 bufcount, FLASH_SIZE_T buflen, UINT8 **buf, void *op)
{
	Ctlr *ctlr;
	UINT32 i;

	printf("%s(flashchip=%p, addr=%lx, bufcount=%d, buflen=%lu, buf=%p, op=%p)\n",
	    __func__, flashchip, (ULONG)addr, bufcount, (ULONG)buflen, buf, op);

	if (addr >= CHIPSIZE || addr + (bufcount * buflen) > CHIPSIZE)
		return ERROR;

	for (i = 0; i < bufcount; i++) {
		if (buf[i] == NULL)
			return ERROR;
	}

	ctlr = vxbDevSoftcGet(flashchip->pDev);
	for (i = 0; i < bufcount; i++) {
		memcpy(ctlr->mem + addr + (buflen * i), buf[i], buflen);
	}
}

LOCAL STATUS
dflashblkerase(FLASH_CHIP_ID flashchip, FLASH_ADDR_T addr, UINT32 blks)
{
	Ctlr *ctlr;

	printf("%s(flashchip=%p, addr=%lx, blocks=%d)\n", __func__, flashchip, (ULONG)addr, blks);

	if (addr >= CHIPSIZE || (addr + blks * SECTSIZE) > CHIPSIZE)
		return ERROR;

	ctlr = vxbDevSoftcGet(flashchip->pDev);
	memset(ctlr->mem + addr, 0xff, blks * SECTSIZE);

	return OK;
}

LOCAL STATUS
dflashshow(VXB_DEV_ID dev, UINT32 verbose)
{
	printf("%s(dev=%p, verbose=%d)\n", __func__, dev, verbose);
	return OK;
}

LOCAL void
norwrite(NOR_FLASH_INFO *norinfo, ULONG addr, UINT32 data)
{
	printf("%s(norinfo=%p, addr=%lx, data=%x)\n", __func__, norinfo, addr, data);
}

LOCAL UINT32
norread(NOR_FLASH_INFO *norinfo, ULONG addr, UINT32 flag)
{
	printf("%s(norinfo=%p, addr=%lx, flag=%x)\n", __func__, norinfo, flag);
	return 0;
}

LOCAL STATUS
nordatacomp(NOR_FLASH_INFO *norinfo, ULONG addr, UINT32 count, char *buf, UINT32 flag)
{
	printf("%s(norinfo=%p, addr=%lx, count=%u, buf=%p, flag=%x)\n",
	    __func__, norinfo, addr, count, buf, flag);
	return OK;
}

LOCAL STATUS
norenable(NOR_FLASH_INFO *norinfo, BOOL enable)
{
	printf("%s(norinfo=%p, enable=%d)\n", __func__, norinfo, enable);
	return OK;
}

LOCAL STATUS
dflashattach(VXB_DEV_ID dev)
{
	VXB_FLASHCTRL *flash;
	NOR_FLASH_INFO *norinfo;
	FLASH_CHIP_ID flashchip;
	Ctlr *ctlr;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	ctlr->alloc = vxbMemAlloc(CHIPSIZE + ALIGN);
	if (!ctlr->alloc)
		goto error;
	ctlr->mem = alignaddr(ctlr->alloc, ALIGN);

	flashchip = &ctlr->flashchip;
	flashchip->pDev = dev;
	flashchip->uChipSize = CHIPSIZE;
	flashchip->uEraseSize = SECTSIZE;
	flashchip->uFlashType = FLASH_CHIP_TYPE_NOR;
	flashchip->uCapability = FLASH_CHIP_CAP_RD | FLASH_CHIP_CAP_WR | FLASH_CHIP_CAP_BLKERA;
	flashchip->flashOPs.ena = dflashenable;
	flashchip->flashOPs.rst = dflashreset;
	flashchip->flashOPs.read = dflashread;
	flashchip->flashOPs.write = dflashwrite;
	flashchip->flashOPs.blkErase = dflashblkerase;
	snprintf((char *)flashchip->chipName, sizeof(flashchip->chipName), "dummy_flash_chip");

	flash = &ctlr->flash;
	flash->pDev = dev;
	flash->unitId = vxbDevUnitGet(dev);
	flash->flashChip = flashchip;
	flash->flashShow = dflashshow;
	flash->private = (VIRT_ADDR)ctlr->mem;
	snprintf(flash->devName, sizeof(flash->devName), "dummy_flash");

	norinfo = &ctlr->norinfo;
	norinfo->baseAdrs = (VIRT_ADDR)ctlr->mem;
	norinfo->maxSize = CHIPSIZE;
	norinfo->portWidthDef = 32;
	norinfo->chipWidthDef = 16;
	norinfo->pDev = dev;
	norinfo->chipEna = norenable;
	norinfo->dataRd = norread;
	norinfo->dataWr = norwrite;
	norinfo->dataComp = nordatacomp;
	snprintf((char *)norinfo->chipName, sizeof(norinfo->chipName), "%s", flashchip->chipName);
	flashchip->pFlashInfo = norinfo;

	vxbDevSoftcSet(dev, ctlr);

	if (vxbFlashChipAdd(flash) != OK)
		goto error;

	return OK;

error:
	dflashfree(ctlr);
	return ERROR;
}

LOCAL VXB_DRV_METHOD dflashdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dflashprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dflashattach },
	VXB_DEVMETHOD_END,
};

VXB_DRV dflashdrv = {
	{ NULL },
	"dummy_flash",
	"Dummy Flash driver",
	VXB_BUSID_FDT,
	0,
	0,
	dflashdev,
	NULL,
};

VXB_DRV_DEF(dflashdrv)

STATUS
dflashdrvadd(void)
{
	return vxbDrvAdd(&dflashdrv);
}

void
dflashlink(void)
{
}

void *
dflashmem(int unit)
{
	VXB_DEV_ID dev;
	Ctlr *ctlr;
	void *mem;

	dev = vxbDevAcquireByName("dummy_flash", unit);
	if (!dev)
		return NULL;

	ctlr = vxbDevSoftcGet(dev);
	mem = ctlr->mem;
	vxbDevRelease(dev);

	return mem;
}
