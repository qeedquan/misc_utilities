#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <ftd2xx.h>

void *
xcalloc(size_t nmemb, size_t size)
{
	void *p;

	if (nmemb == 0)
		nmemb = 1;
	if (size == 0)
		size = 1;

	p = calloc(nmemb, size);
	if (!p)
		abort();
	return p;
}

DWORD
list_devices(void)
{
	FT_DEVICE_LIST_INFO_NODE *dev;
	DWORD i, ndev;
	FT_STATUS r;

	r = FT_CreateDeviceInfoList(&ndev);
	if (r != FT_OK)
		errx(1, "Failed to create device list\n");

	printf("Number of devices: %d\n", ndev);

	dev = xcalloc(ndev, sizeof(*dev));
	if (FT_GetDeviceInfoList(dev, &ndev) != FT_OK)
		errx(1, "Failed to get device info list");

	for (i = 0; i < ndev; i++) {
		printf("Dev %d:\n", i);
		printf("  Flags=0x%x\n", dev[i].Flags);
		printf("  Type=0x%x\n", dev[i].Type);
		printf("  ID=0x%x\n", dev[i].ID);
		printf("  LocId=0x%x\n", dev[i].LocId);
		printf("  SerialNumber=%s\n", dev[i].SerialNumber);
		printf("  Description=%s\n", dev[i].Description);
		printf("  ftHandle=%p\n", dev[i].ftHandle);
	}

	return ndev;
}

int
main(void)
{
	FT_HANDLE h;
	DWORD i, ndev, nr;
	FT_STATUS r;
	BYTE buf[1];

	ndev = list_devices();
	for (i = 0; i < ndev; i++) {
		r = FT_Open(i, &h);
		if (r != FT_OK) {
			printf("failed to open device %d: %d\n", i, r);
			continue;
		}

		r = FT_SetBaudRate(h, 115200);
		if (r != FT_OK)
			printf("failed to set baud rate: %d\n", r);

		r = FT_SetBitMode(h, 0, FT_BITMODE_ASYNC_BITBANG);
		if (r != FT_OK)
			printf("failed to set bitmode: %d\n", r);

		FT_Read(h, buf, sizeof(buf), &nr);
		printf("%d %x\n", nr, buf[0]);

		FT_Close(h);
	}

	return 0;
}
