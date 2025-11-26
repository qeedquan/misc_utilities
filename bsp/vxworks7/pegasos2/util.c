#include "dat.h"
#include "fns.h"

MMU_HASH_TBL *mmuPpcHashTblAddrGet(void);
UINT mmuPpcSrGet(UINT);
UINT mmuPpcSdr1Get(void);

time_t time0;

void
utilinit(void)
{
	time0 = time(NULL);
	tyBackspaceSet(0x7f);
}

time_t
uptime(void)
{
	time_t t;

	t = time(NULL) - time0;
	printf("%ld\n", (long)t);
	return t;
}

void
meminfo(void)
{
	PHYS_MEM_DESC *physmem;
	MMU_HASH_TBL *mht;
	UINT i;

	// SDR1 specifies the base address and the size page tables in memory
	printf("SDR1: %#x\n", mmuPpcSdr1Get());
	printf("\n");

	// Segment Registers
	// The 4 high order bits of a program memory reference's 32-bit logical address index a set of segment registers
	// Each segment register contains a 24-bit virtual segment identifier (VSID)
	// The logical address and VSID are concatenated to create a virtual address
	for (i = 0; i < 16; i++)
		printf("SR #%u: %#x\n", i, mmuPpcSrGet(i));
	printf("\n");

	mht = mmuPpcHashTblAddrGet();
	printf("MMU Hash Table {\n");
	printf("\thTabOrg: %#x\n", mht->hTabOrg);
	printf("\thTabMask: %#x\n", mht->hTabMask);
	printf("\tpteTableSize: %#x\n", mht->pteTableSize);
	printf("}\n");
	printf("\n");

	printf("MMU Page Size: %#x\n", MMU_PAGE_SIZE);
	printf("\n");

	printf("Physical Memory Count: %d\n", sysPhysMemDescNumEnt);
	for (i = 0; i < sysPhysMemDescNumEnt; i++) {
		physmem = sysPhysMemDesc + i;
		printf("Virtual: %#llx Physical: %#llx Length: %#zx Mask: %#x State: %#x\n",
		    (unsigned long long)physmem->virtualAddr, (unsigned long long)physmem->physicalAddr, physmem->len,
		    physmem->initialStateMask, physmem->initialState);
	}
	printf("\n");
}

STATUS
spddata(UINT8 data[256])
{
	VXB_DEV_ID dev;
	I2C_MSG msg[2];
	UINT8 buf[1];
	STATUS status;

	memset(data, 0xff, 256);
	dev = vxbDevAcquireByName("via-i2c", 0);
	if (dev == NULL)
		return ERROR;

	memset(msg, 0, sizeof(msg));
	memset(buf, 0, sizeof(buf));

	msg[0].addr = 0x57;
	msg[0].flags = I2C_M_WR;
	msg[0].buf = buf;
	msg[0].len = sizeof(buf);

	msg[1].addr = 0x57;
	msg[1].flags = I2C_M_RD;
	msg[1].buf = data;
	msg[1].len = 256;

	status = VXB_I2C_XFER(dev, msg, NELEMENTS(msg));

	vxbDevRelease(dev);
	return status;
}

UINT64
tbr(void)
{
	UINT64 lo, hi;

	lo = sysTimeBaseLGet();
	hi = sysTimeBaseUGet();
	return lo | (hi << 32);
}

void
tbw(UINT64 val)
{
	ULONG lo, hi;

	lo = val & 0xffffffff;
	hi = (val >> 32) & 0xffffffff;
	sysTimeBaseLPut(lo);
	sysTimeBaseUPut(hi);
}

void
utillink(void)
{
}
