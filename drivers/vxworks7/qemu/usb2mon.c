/*

USB class driver that reports events on attach/unattach/etc

If there is an existing class driver that matches on the same vendor/product/bcdusbor id, it will fail to register.
Only one driver is expected to manage that device class.

*/

#include <vxWorks.h>
#include <semLib.h>
#include <logLib.h>
#include <errno.h>
#include <errnoLib.h>
#include <iosLib.h>
#include <rngLib.h>
#include <selectLib.h>
#include <wdLib.h>
#include <usbHst.h>
#include <usbd.h>
#include <usb2Kbd.h>
#include <evdevLib.h>
#include <evdevLibKbdMap.h>
#include <tyLib.h>
#include <usb2Hid.h>

typedef struct {
	// generic class info
	USB2_CLASS_DEVICE *classdev;

	// in transfer urb
	USBHST_URB *inurb;

	// endpoint addr
	UINT8 endpoint;
} USB2_MON_DEVICE;

LOCAL USB2_CLASS_DRIVER *usb2mon;

LOCAL USBHST_STATUS
usb2monadd(UINT32 device, UINT8 interface, UINT8 speed, void **driver)
{
	printf("%s(%d)\n", __func__, device);
	return USBHST_SUCCESS;
}

LOCAL void
usb2monrem(UINT32 device, void *driver)
{
	printf("%s(%d)\n", __func__, device);
}

LOCAL void
usb2monsr(UINT32 device, void *driver)
{
	printf("%s(device=%#x)\n", __func__, device);
}

STATUS
usb2moninit(void)
{
	STATUS status;

	status = OK;
	if (usb2mon) {
		printf("%s: Already initialized\n");
		return status;
	}

	usb2mon = usb2ClassDriverCreate("/usb2mon/");
	if (!usb2mon) {
		printf("%s: Failed to create class driver\n", __func__);
		goto error2;
	}

	if (usb2TakeDriver(usb2mon) != OK) {
		printf("Failed to take driver\n", __func__);
		goto error2;
	}

	usb2mon->hstDriver.bFlagVendorSpecific = FALSE;
	usb2mon->hstDriver.uVendorIDorClass = USBD_NOTIFY_ALL;
	usb2mon->hstDriver.uProductIDorSubClass = USBD_NOTIFY_ALL;
	usb2mon->hstDriver.uBCDUSBorProtocol = USBD_NOTIFY_ALL;
	usb2mon->hstDriver.addDevice = usb2monadd;
	usb2mon->hstDriver.removeDevice = usb2monrem;
	usb2mon->hstDriver.suspendDevice = usb2monsr;
	usb2mon->hstDriver.resumeDevice = usb2monsr;

	if (usbHstDriverRegister(&usb2mon->hstDriver, NULL, "usb2mon") != USBHST_SUCCESS) {
		printf("Failed to register class driver\n");
		goto error1;
	}

	usb2ReleaseDriver(usb2mon);

	if (0) {
	error1:
		if (usb2mon)
			usb2ReleaseDriver(usb2mon);
	error2:
		if (usb2mon) {
			usb2ClassDriverDelete(usb2mon);
			usb2mon = NULL;
		}
		status = ERROR;
	}

	return status;
}

void
usb2mondeinit(void)
{
	if (usb2TakeDriver(usb2mon) != OK) {
		printf("%s(): driver mutex not taken\n", __func__);
		return;
	}

	if (usbHstDriverDeregister(&usb2mon->hstDriver) != USBHST_SUCCESS) {
		printf("%s(): driver unregister failed\n", __func__);
		usb2ReleaseDriver(usb2mon);
		return;
	}

	while (!usb2DriverIsEmpty(usb2mon)) {
		usb2ReleaseDriver(usb2mon);
		taskDelay(sysClkRateGet());
		if (usb2TakeDriver(usb2mon) != OK) {
			printf("%s(): driver mutex not taken while emptying\n", __func__);
			return;
		}
	}

	usb2ClassDriverDelete(usb2mon);
	usb2mon = NULL;
}
