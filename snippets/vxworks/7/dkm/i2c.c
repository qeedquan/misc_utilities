#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbI2cLib.h>

STATUS
i2cdetect(char *devname, int unit, int start, int end)
{
	VXB_DEV_ID dev;
	I2C_MSG msg;
	STATUS status;
	int addr;
	int ndev;

	dev = vxbDevAcquireByName(devname, unit);
	if (dev == NULL)
		return ERROR;

	memset(&msg, 0, sizeof(msg));
	printf("Detecting I2C devices...\n");
	ndev = 0;
	for (addr = start; addr <= end; addr++) {
		msg.addr = addr;
		msg.flags = I2C_M_WR;

		// vxbI2cDevXfer works on the child device node, meaning it gets the parent method xfer to use
		// since the device we pass here is the i2c device itself, we need to call it directly with VXB_I2C_XFER
		status = VXB_I2C_XFER(dev, &msg, 1);
		if (status == OK) {
			printf("%#x ", addr);
			ndev++;
		}
	}
	printf("\n");
	printf("Found %d devices\n", ndev);
	vxbDevRelease(dev);
	return OK;
}
