/*

fw_cfg allows QEMU to pass information to the emulated target easily (IO ports or MMIO that is easily readable)

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <sysLib.h>
#include <ioLib.h>
#include <iosLib.h>
#include <endian.h>

enum {
	FW_CFG_SIGNATURE = 0x00,
	FW_CFG_ID = 0x01,
	FW_CFG_UUID = 0x02,
	FW_CFG_RAM_SIZE = 0x03,
	FW_CFG_NOGRAPHIC = 0x04,
	FW_CFG_NB_CPUS = 0x05,
	FW_CFG_MACHINE_ID = 0x06,
	FW_CFG_KERNEL_ADDR = 0x07,
	FW_CFG_KERNEL_SIZE = 0x08,
	FW_CFG_KERNEL_CMDLINE = 0x09,
	FW_CFG_INITRD_ADDR = 0x0a,
	FW_CFG_INITRD_SIZE = 0x0b,
	FW_CFG_BOOT_DEVICE = 0x0c,
	FW_CFG_NUMA = 0x0d,
	FW_CFG_BOOT_MENU = 0x0e,
	FW_CFG_MAX_CPUS = 0x0f,
	FW_CFG_KERNEL_ENTRY = 0x10,
	FW_CFG_KERNEL_DATA = 0x11,
	FW_CFG_INITRD_DATA = 0x12,
	FW_CFG_CMDLINE_ADDR = 0x13,
	FW_CFG_CMDLINE_SIZE = 0x14,
	FW_CFG_CMDLINE_DATA = 0x15,
	FW_CFG_SETUP_ADDR = 0x16,
	FW_CFG_SETUP_SIZE = 0x17,
	FW_CFG_SETUP_DATA = 0x18,
	FW_CFG_FILE_DIR = 0x19,

	FW_CFG_FILE_FIRST = 0x20,
};

enum {
	SELECT = 0x510,
	DATA = 0x511,
};

LOCAL UINT16
read2(UINT16 reg)
{
	UINT8 val[2];
	size_t i;

	sysOutWord(SELECT, reg);
	for (i = 0; i < NELEMENTS(val); i++)
		val[i] = sysInByte(DATA);
	return le16dec(val);
}

LOCAL UINT32
read4(UINT16 reg)
{
	UINT8 val[4];
	size_t i;

	sysOutWord(SELECT, reg);
	for (i = 0; i < NELEMENTS(val); i++)
		val[i] = sysInByte(DATA);
	return le32dec(val);
}

LOCAL UINT64
read8(UINT16 reg)
{
	UINT8 val[8];
	size_t i;

	sysOutWord(SELECT, reg);
	for (i = 0; i < NELEMENTS(val); i++)
		val[i] = sysInByte(DATA);
	return le64dec(val);
}

void
fwcfgdump(void)
{
	printf("FWCFG\n");
	printf("Signature     %#x\n", read4(FW_CFG_SIGNATURE));
	printf("ID            %#x\n", read4(FW_CFG_ID));
	printf("CPUs:         %d\n", read2(FW_CFG_NB_CPUS));
	printf("Max CPUs:     %d\n", read2(FW_CFG_MAX_CPUS));
	printf("RAM           %#lx\n", read8(FW_CFG_RAM_SIZE));
	printf("Kernel Addr   %#lx\n", read8(FW_CFG_KERNEL_ADDR));
	printf("Kernel Entry  %#lx\n", read8(FW_CFG_KERNEL_ENTRY));
	printf("Kernel Size   %#x\n", read4(FW_CFG_KERNEL_SIZE));
	printf("Cmdline Addr  %#x\n", read4(FW_CFG_CMDLINE_ADDR));
	printf("Cmdline Size  %#x\n", read4(FW_CFG_CMDLINE_SIZE));
}
