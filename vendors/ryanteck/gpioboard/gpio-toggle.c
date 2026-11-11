// https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <getopt.h>
#include <err.h>

enum {
	INPUT = 'I',
	OUTPUT = 'O',
	LOW = '0',
	HIGH = '1',
	PULLDOWN = 'D',
	PULLUP = 'U',
	PULLNONE = 'N',
};

typedef struct {
	int baudrate;
	int bits;
	int stopbit;
	int parity;
	int delay;
	int channel;
} Option;

Option opt = {
    .baudrate = 230400,
    .bits = 8,
    .stopbit = 0,
    .parity = 0,
    .delay = 1,
    .channel = 22,
};

void
usage(void)
{
	fprintf(stderr, "usage: <device> <channel>\n");
	fprintf(stderr, "\t-b\tset baud rate (default: %d)\n", opt.baudrate);
	fprintf(stderr, "\t-d\tset delay between toggle (default: %d)\n", opt.delay);
	fprintf(stderr, "\t-h\tshow this message\n");
	fprintf(stderr, "\t-n\tset number of bits (default: %d)\n", opt.bits);
	fprintf(stderr, "\t-p\tset parity (default: %d)\n", opt.parity);
	fprintf(stderr, "\t-s\tset stop bit(default: %d)\n", opt.stopbit);
	exit(2);
}

void
parseopt(Option *o, int *argc, char ***argv)
{
	int c;

	while ((c = getopt(*argc, *argv, "b:d:hn:p:s:")) != -1) {
		switch (c) {
		case 'b':
			o->baudrate = atoi(optarg);
			break;

		case 'd':
			o->delay = atoi(optarg);
			break;

		case 'h':
			usage();
			break;

		case 'n':
			o->bits = atoi(optarg);
			break;

		case 'p':
			o->parity = atoi(optarg);
			break;

		case 's':
			o->stopbit = atoi(optarg);
			break;
		}
	}

	*argc -= optind;
	*argv += optind;

	if (*argc >= 2)
		o->channel = atoi(*argv[1]);
}

int
serialopen(const char *name, Option *o)
{
	struct termios tty;
	int fd;

	fd = open(name, O_RDWR);
	if (fd < 0)
		goto error;

	if (tcgetattr(fd, &tty) != 0)
		goto error;

	tty.c_cflag &= ~(CSIZE | CSTOPB | PARENB);
	switch (o->bits) {
	case 5:
		tty.c_cflag |= CS5;
		break;
	case 6:
		tty.c_cflag |= CS6;
		break;
	case 7:
		tty.c_cflag |= CS7;
		break;
	case 8:
		tty.c_cflag |= CS8;
		break;
	default:
		errno = EINVAL;
		goto error;
	}
	if (o->stopbit)
		tty.c_cflag |= CSTOPB;
	if (o->parity)
		tty.c_cflag |= PARENB;

	if (cfsetspeed(&tty, o->baudrate) < 0)
		goto error;

	if (tcsetattr(fd, TCSANOW, &tty) != 0)
		goto error;

	if (0) {
	error:
		if (fd >= 0)
			close(fd);
		fd = -1;
	}

	return fd;
}

void
setchannel(int fd, int chan, int mode)
{
	int pin;

	pin = chan + 'a';
	dprintf(fd, "%c%c", pin, mode);
	dprintf(fd, "%c%c", pin, PULLNONE);
	dprintf(fd, "%c%c", pin, LOW);
}

void
output(int fd, int chan, int val)
{
	int pin;

	pin = chan + 'a';
	dprintf(fd, "%c%c", pin, val);
}

int
main(int argc, char *argv[])
{
	int fd;

	parseopt(&opt, &argc, &argv);
	if (argc < 1)
		usage();

	fd = serialopen(argv[0], &opt);
	if (fd < 0)
		err(1, "Failed to open serial device %s", argv[0]);

	setchannel(fd, opt.channel, OUTPUT);
	for (;;) {
		output(fd, opt.channel, LOW);
		sleep(opt.delay);
		output(fd, opt.channel, HIGH);
		sleep(opt.delay);
	}

	close(fd);
	return 0;
}
