/*

For a DTS node that is for purely reading/writing registers, use the syscon driver.
The syscon driver is a dummy driver that attaches itself to the VxBus so we can get the node to it.
Add another compatible name that doesn't match a driver so we can use the VxBus API to get the node using a unique name.

DTS:

test_regmap: test_regmap@b80000 {
    compatible = "test_regmap", "syscon";
    reg = <0 0xb80000 0 0x1000>;

    // puts restriction on a specific reg width we can do read/writes on.
    // for example, using <4> means only 32 bit read/writes work.
    reg-io-width = <4>;

    // endian of the read/writes
    // can be little-endian, big-endian, native-endian, etc.
    native-endian;
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <inttypes.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <subsys/reg/vxbRegMap.h>

STATUS
test_vxbregmap(char *name)
{
	VXB_REG_MAP *regmap;
	STATUS r;
	uint_t addr;
	uint8_t val8;
	uint16_t val16;
	uint32_t val32;
	uint64_t val64;
	int rmask;
	int wmask;
	int offset;

	r = OK;
	offset = vxFdtNodeOffsetByCompatible(0, name);
	if (offset < 0) {
		printf("failed to get offset\n");
		goto error;
	}

	if (vxbRegMapGet(offset, &regmap) != OK) {
		printf("failed to get regmap\n");
		goto error;
	}

	for (addr = 0; addr <= 32; addr++) {
		val8 = 0;
		val16 = 0;
		val32 = 0;
		val64 = 0;
		rmask = 0;
		wmask = 0;
		if (vxbRegMapRead8(regmap, addr, &val8) != OK)
			rmask |= 0x1;
		if (vxbRegMapRead16(regmap, addr, &val16) != OK)
			rmask |= 0x2;
		if (vxbRegMapRead32(regmap, addr, &val32) != OK)
			rmask |= 0x4;
		if (vxbRegMapRead64(regmap, addr, &val64) != OK)
			rmask |= 0x8;

		if (vxbRegMapWrite8(regmap, addr, val8 + 1) != OK)
			wmask |= 0x1;
		if (vxbRegMapWrite16(regmap, addr, val16 + 1) != OK)
			wmask |= 0x2;
		if (vxbRegMapWrite32(regmap, addr, val32 + 1) != OK)
			wmask |= 0x4;
		if (vxbRegMapWrite64(regmap, addr, val64 + 1) != OK)
			wmask |= 0x8;

		printf("addr %#04x rmask %#04x wmask %#04x | %#x %#x %#" PRIx32 " %#" PRIx64 "\n",
		       addr, rmask, wmask, val8, val16, val32, val64);
	}

	if (0) {
	error:
		r = ERROR;
	}

	return r;
}
