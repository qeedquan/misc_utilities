#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

void
test_local_version(int fd)
{
	struct hci_version ver;
	int r;

	r = hci_read_local_version(fd, &ver, 1000);
	if (r < 0) {
		fprintf(stderr, "Failed to read local version: %s\n", strerror(-r));
		return;
	}

	printf("Local version\n");
	printf("Manufacturer   %#x\n", ver.manufacturer);
	printf("HCI Version    %#x\n", ver.hci_ver);
	printf("HCI Revision   %#x\n", ver.hci_rev);
	printf("LMP Version    %#x\n", ver.lmp_ver);
	printf("LMP Subversion %#x\n", ver.lmp_subver);
	printf("\n");
}

void
test_local_features(int fd)
{
	uint8_t feature[1024];
	size_t i;
	int r;

	memset(feature, 0, sizeof(feature));
	r = hci_read_local_features(fd, feature, 1000);
	if (r < 0) {
		fprintf(stderr, "Failed to read local version: %s\n", strerror(-r));
		return;
	}

	printf("Local feature\n");
	for (i = 0; i < sizeof(feature); i++) {
		if (feature[i] == 0)
			break;
		printf("%#x\n", feature[i]);
	}
	printf("\n");
}

void
test_bdaddr(int fd)
{
	bdaddr_t addr;
	char str[128];
	int r;

	r = hci_read_bd_addr(fd, &addr, 1000);
	if (r < 0) {
		fprintf(stderr, "Failed to read BD address: %s\n", strerror(-r));
		return;
	}
	ba2str(&addr, str);
	printf("Bluetooth address\n");
	printf("%s\n", str);
	printf("\n");
}

void
test_le_whitelist(int fd)
{
	uint8_t size;
	int r;

	r = hci_le_read_white_list_size(fd, &size, 1000);
	if (r < 0) {
		fprintf(stderr, "Failed to read white list size: %s\n", strerror(-r));
		return;
	}

	printf("LE Whitelist\n");
	printf("Size: %d\n", size);
	printf("\n");
}

int
main(int argc, char *argv[])
{
	int fd, dev;

	dev = 0;
	if (argc >= 2)
		dev = atoi(argv[1]);

	fd = hci_open_dev(dev);
	if (fd < 0)
		errx(1, "Failed to open HCI: %s", strerror(-fd));

	test_local_version(fd);
	test_local_features(fd);
	test_bdaddr(fd);
	test_le_whitelist(fd);

	hci_close_dev(fd);
	return 0;
}
