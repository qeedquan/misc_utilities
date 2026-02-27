#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <libudev.h>

char *
batocomp(const bdaddr_t *ba)
{
	struct udev *udev;
	struct udev_hwdb *hwdb;
	struct udev_list_entry *head, *entry;
	char modalias[11], *comp = NULL;

	sprintf(modalias, "OUI:%2.2X%2.2X%2.2X", ba->b[5], ba->b[4], ba->b[3]);

	udev = udev_new();
	if (!udev)
		return NULL;

	hwdb = udev_hwdb_new(udev);
	if (!hwdb)
		goto done;

	head = udev_hwdb_get_properties_list_entry(hwdb, modalias, 0);

	udev_list_entry_foreach(entry, head)
	{
		const char *name = udev_list_entry_get_name(entry);

		if (name && !strcmp(name, "ID_OUI_FROM_DATABASE")) {
			comp = strdup(udev_list_entry_get_value(entry));
			break;
		}
	}

	hwdb = udev_hwdb_unref(hwdb);

done:
	udev = udev_unref(udev);

	return comp;
}

void
dump(const char *name)
{
	struct hci_dev_info di;
	struct hci_version ver;
	bdaddr_t bdaddr;
	int dev, fd;
	char addr[64], *comp;

	comp = NULL;
	dev = 0;
	if (name && (dev = hci_devid(name)) < 0) {
		fprintf(stderr, "invalid device id '%s'\n", name);
		return;
	}

	if ((fd = hci_open_dev(dev)) < 0) {
		fprintf(stderr, "failed to open device");
		goto error;
	}

	if (hci_devinfo(dev, &di) < 0) {
		fprintf(stderr, "failed to get device info");
		goto error;
	}

	if (hci_read_local_version(fd, &ver, 1000) < 0) {
		fprintf(stderr, "failed to read version info");
		goto error;
	}

	if (!bacmp(&di.bdaddr, BDADDR_ANY)) {
		if (hci_read_bd_addr(fd, &bdaddr, 1000) < 0) {
			fprintf(stderr, "failed to read address");
			goto error;
		}
	} else
		bacpy(&bdaddr, &di.bdaddr);

	ba2str(&bdaddr, addr);
	comp = batocomp(&bdaddr);

	printf("Manufacturer:   %s (%d)\n", bt_compidtostr(ver.manufacturer), ver.manufacturer);
	printf("Device address: %s", addr);
	if (comp)
		printf(" (%s)\n", comp);
	else
		printf("\n");

	if (0) {
	error:
		fprintf(stderr, "for hci%d: %s (%d)\n", dev, strerror(errno), errno);
	}

	if (fd >= 0)
		hci_close_dev(fd);
	free(comp);
}

void
usage(void)
{
	fprintf(stderr, "usage: <device> ...\n");
	fprintf(stderr, "\nExample Devices: hci0\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	int i, c;

	while ((c = getopt(argc, argv, "h")) != -1) {
		switch (c) {
		case 'h':
		default:
			usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (argc < 1)
		dump(NULL);
	else {
		for (i = 0; i < argc; i++)
			dump(argv[i]);
	}
	return 0;
}
