#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <err.h>
#include <can.h>

void
usage(void)
{
	fprintf(stderr, "usage: [options] <interface>\n");
	fprintf(stderr, "  -h    show this message\n");
	fprintf(stderr, "  -r    rx mode\n");
	fprintf(stderr, "  -t    tx mode\n");
	exit(2);
}

void
printframe(const char *op, struct can_frame *frame)
{
	int i;

	printf("%s: %03X [%d] ", op, frame->can_id, frame->can_dlc);
	for (i = 0; i < frame->can_dlc; i++)
		printf("%02X ", frame->data[i]);
	printf("\n");
}

void
rx(int fd)
{
	struct can_frame frame;
	ssize_t nbytes;

	for (;;) {
		nbytes = recv(fd, &frame, sizeof(frame), 0);
		if (nbytes != sizeof(frame)) {
			perror("recv");
			break;
		}
		printframe("recv", &frame);
	}
}

void
tx(int fd)
{
	struct can_frame frame;
	ssize_t nbytes;
	int i;

	for (;;) {
		memset(&frame, 0, sizeof(frame));
		frame.can_id = rand() & 0x7ff;
		frame.can_dlc = rand() & 0x7;
		for (i = 0; i < frame.can_dlc; i++)
			frame.data[i] = rand() & 0xff;

		printframe("send", &frame);

		nbytes = send(fd, &frame, sizeof(frame), 0);
		if (nbytes != sizeof(frame)) {
			perror("send");
			break;
		}
		sleep(1);
	}
}

int
main(int argc, char *argv[])
{
	struct ifreq ifr;
	struct sockaddr_can addr;
	int fd;
	int mode;
	int c;

	mode = 'r';
	while ((c = getopt(argc, argv, "hrt")) != -1) {
		switch (c) {
		case 'h':
			usage();
			break;
		case 'r':
		case 't':
			mode = c;
			break;
		}
	}
	argv += optind;
	argc -= optind;
	if (argc < 1)
		usage();

	fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (fd < 0)
		err(1, "socket");

	snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", argv[0]);
	if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)
		err(1, "ioctl");

	addr.can_family = PF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
		err(1, "bind");

	time(NULL);
	if (mode == 'r')
		rx(fd);
	else
		tx(fd);

	close(fd);

	return 0;
}
