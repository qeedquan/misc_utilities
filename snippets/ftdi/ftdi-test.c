#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <getopt.h>
#include <ftdi.h>
#include <libusb.h>

typedef struct {
	int baudrate;
	int vid;
	int did;
	int index;
	int port;
} Option;

Option opt = {
	.baudrate = 115200,
	.vid = 0x0403,
	.did = 0x6010,
	.index = 0,
	.port = 0,
};

// we want to reattach on close so override the default close function
int
xftdi_usb_close(struct ftdi_context *ftdi)
{
	int r;

	r = 0;
	if (ftdi == NULL) {
		ftdi->error_str = "ftdi_context invalid";
		return -3;
	}

	if (ftdi->usb_dev != NULL) {
		if (libusb_release_interface(ftdi->usb_dev, ftdi->interface) < 0)
			r = -1;
		if (libusb_attach_kernel_driver(ftdi->usb_dev, ftdi->interface) < 0)
			r = -1;

		libusb_close(ftdi->usb_dev);
		ftdi->usb_dev = NULL;
	}

	return r;
}

void
usage(void)
{
	fprintf(stderr, "usage: [options]\n");
	fprintf(stderr, "  -b <baudrate>    specify baud rate (default: %d)\n", opt.baudrate);
	fprintf(stderr, "  -d <vid>:<did>   specify vendor and device id (default: %04x:%04x)\n", opt.vid, opt.did);
	fprintf(stderr, "  -i <index>       specify device index (default: %d)\n", opt.index);
	fprintf(stderr, "  -p <port>        specify port (default: %d)\n", opt.port);
	exit(2);
}

void
test_bitbang_read(struct ftdi_context *ftdi)
{
	uint8_t buf[1];
	int r, nr;

	if ((r = ftdi_set_bitmode(ftdi, 0, BITMODE_BITBANG)) < 0)
		errx(1, "Failed to set bitbang mode: %s\n", ftdi_get_error_string(ftdi));

	printf("kernel active %d\n", libusb_kernel_driver_active(ftdi->usb_dev, ftdi->interface));
	memset(buf, 0, sizeof(buf));
	nr = ftdi_read_data(ftdi, buf, sizeof(buf));
	if (nr < 0)
		errx(1, "Failed to read data: %s\n", ftdi_get_error_string(ftdi));

	printf("usbdev %p index %d port %d interface %d data %#x\n", ftdi->usb_dev, opt.index, opt.port, ftdi->interface, buf[0]);
}

int
main(int argc, char *argv[])
{
	struct ftdi_context *ftdi;
	struct ftdi_device_list *devs;
	int c, r, ndevs;

	while ((c = getopt(argc, argv, "b:d:i:p:h")) != -1) {
		switch (c) {
		case 'b':
			opt.baudrate = atoi(optarg);
			break;

		case 'd':
			if (sscanf(optarg, "%d:%d", &opt.vid, &opt.did) != 2)
				errx(1, "invalid id specified");
			break;

		case 'i':
			opt.index = atoi(optarg);
			break;

		case 'p':
			opt.port = atoi(optarg);
			break;

		case 'h':
		default:
			usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	ftdi = ftdi_new();
	if (!ftdi)
		errx(1, "Failed to allocate ftdi device");

	if ((ndevs = ftdi_usb_find_all(ftdi, &devs, opt.vid, opt.did)) < 0)
		errx(1, "Failed to find usb devices: %s", ftdi_get_error_string(ftdi));

	printf("Found %d devices\n", ndevs);
	if (opt.index >= ndevs)
		errx(1, "Invalid device index specified");

	if ((r = ftdi_set_interface(ftdi, opt.port)) < 0)
		errx(1, "Failed to set port: %s", ftdi_get_error_string(ftdi));

	if ((r = ftdi_usb_open_desc_index(ftdi, opt.vid, opt.did, NULL, NULL, opt.index)) < 0)
		errx(1, "Failed to open USB device: %s", ftdi_get_error_string(ftdi));

	if ((r = ftdi_set_baudrate(ftdi, opt.baudrate)) < 0)
		errx(1, "Failed to set baud rate: %s", ftdi_get_error_string(ftdi));

	printf("Chip type: %d\n", ftdi->type);

	test_bitbang_read(ftdi);

	if ((r = xftdi_usb_close(ftdi) < 0))
		errx(1, "Failed to close USB device: %s", ftdi_get_error_string(ftdi));

	ftdi_list_free(&devs);
	ftdi_free(ftdi);
	return 0;
}
