/*

Dummy USB Host controller driver

This dummy USB driver tests the host controller/class/etc API

In order to do anything USB related, need to include USB_INIT in VIP for initialization of the USB stack

A USB host controller manages the USB devices connected on the host
A USB class driver are devices like a keyboard/mouse/serial/etc

Hierarchy:
USB Host Controller -> USB Bus (Root Hubs)/USB Class drivers

Host controllers are often allocated separately from the VxBus driver, it is initialized in USB_INIT
The VxBus driver is usually stored inside the host controller

It is a common design for USB >=2.0 ports to be managed by two controllers:
one for low speed and full speed (the USB 1.1 speeds), and one for high speed (the new speed available beginning with USB 2.0).
If this is the case, the 3rd root hub is probably from the EHCI controller managing the high speed connections,
and the 1st and 2nd root hubs are probably from a pair of OHCI or UHCI controllers managing the low and full speed connections.
Each USB port on your computer would then be connected to two of the controllers,
the EHCI controller and one of the other two controllers,
and which one is used would depend on the speed chosen by the USB device connected to that port.

USB has four transfer/endpoint types:
Control - Command and status operations
Interrupt - Under USB if a device requires the attention of the host,
            it must wait until the host polls it before it can report that it needs urgent attention
Isochronous - Isochronous transfers occur continuously and periodically
Bulk - Bulk transfers can be used for large bursty data. Print jobs or an image could be considered bulk

These are packed bits into a field:
USB_ATTR_CONTROL                    0x00
USB_ATTR_ISOCH                      0x01
USB_ATTR_BULK                       0x02
USB_ATTR_INTERRUPT                  0x03

Any USB transaction submits a request via an URB.
An URB consists of all relevant information to execute any USB transaction and deliver the data and status back.

Every USB device must provide at least one control endpoint at address 0 called the default endpoint or Endpoint0.
This endpoint is bidirectional. That is, the host can send data to the endpoint and receive data from it within one transfer.
The purpose of a control transfer is to enable the host to obtain device information, configure the device,
or perform control operations that are unique to the device.

Data endpoints are optional and used for transferring data.
They're unidirectional, has a type (control, interrupt, bulk, isochronous) and other properties.
All those properties are described in an endpoint descriptor (see Standard USB descriptors).

On startup, the USB stack sends a USB SETUP packet to the host controller.

All USB devices have a hierarchy of descriptors which describe to the host information such as what the device is,
who makes it, what version of USB it supports, how many ways it can be configured, the number of endpoints and their types etc

Common USB descriptors:
Device Descriptors
Configuration Descriptors
Interface Descriptors
Endpoint Descriptors
String Descriptors

USB devices can only have one device descriptor. The device descriptor includes information such as what USB revision the device complies to,
the Product and Vendor IDs used to load the appropriate drivers and the number of possible configurations the device can have.
The number of configurations indicate how many configuration descriptors branches are to follow.

USB is little-endian.

DTS:

dummy_usb: dummy_usb@0 {
    compatible = "dummy-usb";
};

*/

#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <hwif/vxBus.h>
#include <hwif/buslib/vxbFdtLib.h>
#include <usbOsal.h>
#include <usbHst.h>
#include <usb2Hid.h>
#include <usb.h>

// data structure for the root hub
typedef struct {
	UINT8 device_address;
} RH_DATA;

// data structure maintained for every host controller
typedef struct {
	UINT32 bus_index;

	// device class type (FDT or PCI)
	VXB_BUSTYPE_ID devclass;

	// default pipe points to the root pipe
	void *default_pipe;

	RH_DATA rh_data;

	// main controller pointer
	void *ctlr;
} HCD_DATA;

typedef struct {
	// speed, according to HOST_TYPE reg
	UINT8 speed;

	// endpoint type
	UINT8 endpoint_type;

	// hub info
	UINT16 hub_info;

	// max packet size
	UINT8 max_packet_size;

	// any possible flags
	UINT32 flags;
} HCD_PIPE;

typedef struct {
	VXB_DEV_ID dev;

	UINT32 host_handle;

	// max host controllers
	HCD_DATA hcd_data[1];

	// max number of pipes
	HCD_PIPE hcd_pipe[16];
	size_t num_hcd_pipe;
} Ctlr;

LOCAL HCD_DATA *g_hcd_data;

LOCAL const VXB_FDT_DEV_MATCH_ENTRY dusbfdt[] = {
	{ "dummy-usb", NULL },
	{ NULL },
};

LOCAL void
print_urb(USBHST_URB *urb)
{
	USBHST_SETUP_PACKET *setup;

	printf("\nURB\n");
	printf("Transfer type: %d buffer %p size: %d callback %p\n",
	    urb->uTransferType, urb->pTransferBuffer, urb->uTransferLength, urb->pfCallback);

	switch (urb->uTransferType) {
	case USB_ATTR_CONTROL:
		setup = urb->pTransferSpecificData;
		printf("Setup Packet: type=%#x, request=%d, value=%d, index=%d, length=%d\n",
		    setup->bmRequestType, setup->bRequest, setup->wValue, setup->wIndex, setup->wLength);
		break;
	}
	printf("\n");
}

LOCAL USBHST_STATUS
pipe_control(UINT8 bus_index, ULONG pipe_handle, USBHST_PIPE_CONTROL_INFO *pipe_ctrl)
{
	printf("%s(bus_index=%d, pipe_handle=%#lx, pipe_ctrl=%p)\n",
	    __func__, bus_index, pipe_handle, pipe_ctrl);
	return USBHST_SUCCESS;
}

LOCAL USBHST_STATUS
create_pipe(UINT8 bus_index, UINT8 device_address, UINT8 device_speed, UCHAR *endpoint, UINT16 high_speed_hub_info, ULONG *pipe_handle)
{
	printf("%s(bus_index=%d, device_address=%d, device_speed=%d, endpoint=%p, high_speed_hub_info=%d, pipe_handle=%p)\n",
	    __func__, bus_index, device_address, device_speed, endpoint, high_speed_hub_info, pipe_handle);

	if (!endpoint || !pipe_handle)
		return USBHST_INVALID_PARAMETER;

	return USBHST_SUCCESS;
}

LOCAL USBHST_STATUS
delete_pipe(UINT8 bus_index, ULONG pipe_handle)
{
	printf("%s(bus_index=%d, pipe_handle=%#lx)\n", __func__, bus_index, pipe_handle);
	return USBHST_SUCCESS;
}

LOCAL USBHST_STATUS
submit_urb(UINT8 bus_index, ULONG pipe_handle, USBHST_URB *urb)
{
	static const USBHST_DEVICE_DESCRIPTOR device_descriptor = {
		.bDescriptorType = USB_DESCR_DEVICE,
		.bMaxPacketSize = 64,
		.idVendor = 0xfbad,
		.idProduct = 0xf00d,
		.bNumConfigurations = 1,
	};

	static const USBHST_CONFIG_DESCRIPTOR config_descriptor = {
		.bDescriptorType = USB_DESCR_CONFIGURATION,
	};

	USBHST_SETUP_PACKET *setup;
	HCD_DATA *hcd_data;
	size_t length;

	printf("%s(bus_index=%d, pipe_handle=%#lx, urb=%p)\n", __func__, bus_index, pipe_handle, urb);

	if (!pipe_handle || !urb)
		return USBHST_INVALID_PARAMETER;

	print_urb(urb);
	hcd_data = &g_hcd_data[bus_index];
	urb->nStatus = USBHST_SUCCESS;
	switch (urb->uTransferType) {
	case USB_ATTR_CONTROL:
		setup = urb->pTransferSpecificData;
		switch (setup->bRequest) {
		case USB_REQ_SET_ADDRESS:
			if (!setup->wValue)
				urb->nStatus = USBHST_INVALID_REQUEST;
			else
				hcd_data->rh_data.device_address = setup->wValue & 0xff;
			break;

		case USB_REQ_GET_DESCRIPTOR:
			switch (setup->wValue) {
			case USB_DESCR_DEVICE:
				length = min(urb->uTransferLength, sizeof(device_descriptor));
				memcpy(urb->pTransferBuffer, &device_descriptor, length);
				break;

			case USB_DESCR_CONFIGURATION:
				length = min(urb->uTransferLength, sizeof(config_descriptor));
				memcpy(urb->pTransferBuffer, &config_descriptor, length);
				break;

			default:
				urb->nStatus = USBHST_INVALID_REQUEST;
				printf("Unhandled get descriptor: %d\n\n", setup->wValue);
				break;
			}
			break;

		default:
			urb->nStatus = USBHST_INVALID_REQUEST;
			printf("Unhandled setup packet request: %d\n\n", setup->bRequest);
			break;
		}
		break;

	default:
		printf("Unhandled URB\n\n");
		urb->nStatus = USBHST_INVALID_REQUEST;
		break;
	}

	if (urb->pfCallback)
		urb->pfCallback(urb);

	return urb->nStatus;
}

LOCAL USBHST_STATUS
cancel_urb(UINT8 bus_index, ULONG pipe_handle, USBHST_URB *urb)
{
	printf("%s(bus_index=%d, pipe_handle=%#lx, urb=%p)\n",
	    __func__, bus_index, pipe_handle, urb);

	urb->nStatus = USBHST_SUCCESS;
	if (urb)
		urb->pfCallback(urb);

	return USBHST_SUCCESS;
}

LOCAL USBHST_STATUS
is_bandwidth_available(UINT8 bus_index, UINT8 device_address, UINT8 device_speed, UCHAR *current_descriptor, UCHAR *new_descriptor)
{
	printf("%s(bus_index=%d)\n", __func__, bus_index);
	return USBHST_SUCCESS;
}

LOCAL USBHST_STATUS
get_frame_number(UINT8 bus_index, UINT16 *frame_number)
{
	printf("%s(bus_index=%d, frame_number=%p)\n", __func__, bus_index, frame_number);
	return USBHST_SUCCESS;
}

LOCAL USBHST_STATUS
set_bit_rate(UINT8 bus_index, BOOL increment, UINT32 *current_frame_width)
{
	printf("%s()\n", __func__);
	return USBHST_SUCCESS;
}

LOCAL USBHST_STATUS
is_request_pending(UINT8 bus_index, ULONG pipe_handle)
{
	printf("%s()\n", __func__);
	return USBHST_SUCCESS;
}

LOCAL USBHST_STATUS
modify_default_pipe(UINT8 bus_index, ULONG default_pipe_handle, UINT8 device_speed, UINT8 max_packet_size, UINT16 high_speed_hub_info)
{
	HCD_PIPE *hcd_pipe;

	printf("%s(bus_index=%d, default_pipe_handle=%#x, device_speed=%d, max_packet_size=%d, high_speed_hub_info=%#x)\n",
	    __func__, bus_index, default_pipe_handle, device_speed, max_packet_size, high_speed_hub_info);

	if (!default_pipe_handle)
		return USBHST_INVALID_PARAMETER;

	// set the default speed and endpoint type of the pipe
	hcd_pipe = (HCD_PIPE *)default_pipe_handle;
	hcd_pipe->speed = device_speed;
	hcd_pipe->hub_info = high_speed_hub_info;
	hcd_pipe->endpoint_type = USB_ATTR_CONTROL;
	hcd_pipe->max_packet_size = max_packet_size;

	return USBHST_SUCCESS;
}

LOCAL USBHST_STATUS
device_add(UINT32 device, UINT8 interface, UINT8 speed, void **driver)
{
	printf("%s(device=%#x, interface=%d, speed=%d)\n", __func__, device, interface, speed);
	return USBHST_SUCCESS;
}

LOCAL void
device_remove(UINT32 device, void *driver)
{
	printf("%s()\n", __func__);
}

LOCAL void
device_suspend_resume(UINT32 device, void *driver)
{
	printf("%s()\n", __func__);
}

LOCAL void
dusbfree(Ctlr *ctlr)
{
	if (!ctlr)
		return;

	vxbMemFree(ctlr);
}

LOCAL int
dusbprobe(VXB_DEV_ID dev)
{
	return vxbFdtDevMatch(dev, dusbfdt, NULL);
}

LOCAL STATUS
dusbattach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;
	USBHST_HC_DRIVER hc;
	USBHST_DEVICE_DRIVER dc;
	HCD_DATA *hcd_data;

	ctlr = vxbMemAlloc(sizeof(*ctlr));
	if (!ctlr)
		goto error;

	ctlr->dev = dev;
	vxbDevSoftcSet(dev, ctlr);

	memset(&hc, 0, sizeof(hc));
	hc.pipeControl = pipe_control;
	hc.createPipe = create_pipe;
	hc.deletePipe = delete_pipe;
	hc.submitURB = submit_urb;
	hc.cancelURB = cancel_urb;
	hc.isBandwidthAvailable = is_bandwidth_available;
	hc.modifyDefaultPipe = modify_default_pipe;
	hc.isRequestPending = is_request_pending;
	hc.getFrameNumber = get_frame_number;
	hc.setBitRate = set_bit_rate;

	memset(&dc, 0, sizeof(dc));
	dc.bFlagVendorSpecific = FALSE;
	dc.uVendorIDorClass = USB2_HID_CLASS;
	dc.uProductIDorSubClass = USBD_NOTIFY_ALL;
	dc.uBCDUSBorProtocol = USBD_NOTIFY_ALL;
	dc.addDevice = device_add;
	dc.removeDevice = device_remove;
	dc.suspendDevice = device_suspend_resume;
	dc.resumeDevice = device_suspend_resume;

	hcd_data = &ctlr->hcd_data[0];
	hcd_data->ctlr = ctlr;
	hcd_data->default_pipe = &ctlr->hcd_pipe[0];
	ctlr->num_hcd_pipe = 1;

	printf("%s(default_pipe=%p)\n", __func__, hcd_data->default_pipe);

	g_hcd_data = ctlr->hcd_data;
	// register a host controller
	if (usbHstHCDRegister(&hc, &ctlr->host_handle, NULL, 0) != USBHST_SUCCESS) {
		printf("Failed to register host controller\n");
		goto error;
	}

	// register a usb bus (root hub) to usbd
	if (usbHstBusRegister(ctlr->host_handle, USB_SPEED_HIGH, (ULONG)hcd_data->default_pipe, dev) != USBHST_SUCCESS) {
		printf("Failed to register a USB bus\n");
		goto error;
	}

	// register a class driver to a usb host
	if (usbHstDriverRegister(&dc, NULL, "dummy_usb_class") != USBHST_SUCCESS) {
		printf("Failed to register class driver\n");
		goto error;
	}

	return OK;

error:
	dusbfree(ctlr);
	return ERROR;
}

LOCAL STATUS
dusbdetach(VXB_DEV_ID dev)
{
	Ctlr *ctlr;

	ctlr = vxbDevSoftcGet(dev);
	dusbfree(ctlr);
	return OK;
}

LOCAL STATUS
dusbshutdown(VXB_DEV_ID dev)
{
	return OK;

	(void)dev;
}

LOCAL VXB_DRV_METHOD dusbdev[] = {
	{ VXB_DEVMETHOD_CALL(vxbDevProbe), dusbprobe },
	{ VXB_DEVMETHOD_CALL(vxbDevAttach), dusbattach },
	{ VXB_DEVMETHOD_CALL(vxbDevDetach), dusbdetach },
	{ VXB_DEVMETHOD_CALL(vxbDevShutdown), dusbshutdown },
	VXB_DEVMETHOD_END,
};

VXB_DRV dusbdrv = {
	{ NULL },
	"dummy-usb",
	"Dummy USB driver",
	VXB_BUSID_FDT,
	0,
	0,
	dusbdev,
	NULL,
};

VXB_DRV_DEF(dusbdrv)

STATUS
dusbdrvadd(void)
{
	STATUS status;

	status = vxbDrvAdd(&dusbdrv);
	return status;
}
