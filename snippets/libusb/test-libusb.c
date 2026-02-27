#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <libusb.h>
#include <getopt.h>

typedef struct {
	int verbose;
} Option;

Option opt = {
	.verbose = 0,
};

void
test_version(void)
{
	const struct libusb_version *v;

	v = libusb_get_version();
	printf("%d.%d.%d.%d\n", v->major, v->minor, v->micro, v->nano);
}

void
test_device_list(libusb_context *c)
{
	libusb_device **list, *dev;
	struct libusb_device_descriptor desc;
	ssize_t i, j, n, m;
	uint8_t port[32];
	int bus, speed, addr;

	if ((n = libusb_get_device_list(c, &list)) < 0) {
		printf("failed to get device list: %zd\n", n);
		return;
	}

	printf("number of device founds: %zd\n\n", n);
	for (i = 0; i < n; i++) {
		dev = list[i];
		m = libusb_get_port_numbers(dev, port, sizeof(port));
		bus = libusb_get_bus_number(dev);
		addr = libusb_get_device_address(dev);
		speed = libusb_get_device_speed(dev);
		if (libusb_get_device_descriptor(dev, &desc) < 0)
			memset(&desc, 0, sizeof(desc));

		printf("bus %03d device %03d speed %d %04x:%04x",
		       bus, addr, speed, desc.idVendor, desc.idProduct);
		if (m) {
			printf(" port ");
			for (j = 0; j < m; j++)
				printf("%d ", port[j]);
		}
		printf("\n");
	}

	libusb_free_device_list(list, 1);
}

void
usage(void)
{
	fprintf(stderr, "usage: [-v]\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	libusb_context *c;
	int r, p;

	while ((p = getopt(argc, argv, "hv")) != -1) {
		switch (p) {
		case 'v':
			opt.verbose = 1;
			break;

		case 'h':
		default:
			usage();
			break;
		}
	}

	r = libusb_init(&c);
	if (r < 0)
		errx(1, "failed to init libusb");

	if (opt.verbose)
		libusb_set_option(c, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_DEBUG);
	
	test_version();
	test_device_list(c);

	libusb_exit(c);

	return 0;
}
