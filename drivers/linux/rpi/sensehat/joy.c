#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include "joy.h"
#include "util.h"

int
joydevs(char ***devs, size_t *len)
{
	return finddevs(devs, len, "/sys/class/input/event*/device/name", "Raspberry Pi Sense HAT Joystick", "/dev/input", 2);
}

int
joyopen(joy_t *joy, const char *dev)
{
	int fd;

	fd = open(dev, O_RDONLY);
	if (fd < 0)
		return -errno;

	joy->fd = fd;
	return 0;
}

void
joyclose(joy_t *joy)
{
	close(joy->fd);
}

int32_t
joyread(joy_t *joy)
{
	struct pollfd fds[1];
	uint8_t data[4 + 4 + 2 + 2 + 4];
	int32_t key, act;

	memset(fds, 0, sizeof(fds));
	fds[0].fd = joy->fd;
	fds[0].events = POLLIN;

	if (poll(fds, 1, 1) <= 0)
		return -1;

	if (read(joy->fd, data, sizeof(data)) != sizeof(data))
		return -errno;

	if (leload(data + 8, 2) != 0x1)
		return -1;

	key = 0;
	switch (leload(data + 10, 2)) {
	case 0x67:
		key = JUP;
		break;
	case 0x69:
		key = JLEFT;
		break;
	case 0x6a:
		key = JRIGHT;
		break;
	case 0x6c:
		key = JDOWN;
		break;
	case 0x1c:
		key = JENTER;
		break;
	}

	act = 0;
	switch (leload(data + 12, 4)) {
	case 0:
		act = JRELEASED;
		break;
	case 1:
		act = JPRESSED;
		break;
	case 2:
		act = JHELD;
		break;
	}

	return key | act;
}
