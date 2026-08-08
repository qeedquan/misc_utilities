#include "vxWorks.h"

int
vxbPciConfigBdfPack(int bus, int device, int func)
{
	return ((bus << 16) & 0x00ff0000) |
	       ((device << 11) & 0x0000f800) |
	       ((func << 8) & 0x00000700);
}
