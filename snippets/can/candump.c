/*

Dump CAN data from an interface

To generate data:
cangen vcan0
candump vcan0

*/

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <unistd.h>
#include <err.h>

int
main(int argc, char *argv[])
{
	struct ifreq ifr;
	struct sockaddr_can addr;
	struct can_frame frame;
	ssize_t nr;
	int fd, i;

	if (argc < 2) {
		printf("usage: <interface>\n");
		return 1;
	}

	fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (fd < 0)
		err(1, "socket");

	memset(&ifr, 0, sizeof(ifr));
	snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", argv[1]);
	if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)
		err(1, "ioctl");

	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		err(1, "bind");

	for (;;) {
		nr = read(fd, &frame, sizeof(frame));
		if (nr < 0)
			err(1, "read");
		if (nr != sizeof(frame))
			break;

		printf("[%zd] [%-4d] [%d] ", nr, frame.can_id, frame.can_dlc);
		for (i = 0; i < frame.can_dlc; i++)
			printf("%02x ", frame.data[i]);
		printf("\n");
	}

	close(fd);
	return 0;
}
