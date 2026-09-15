#include <vxWorks.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <err.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <hwif/drv/watchdog/watchdog.h>

typedef struct {
	volatile sig_atomic_t run;
	int cycles;
	int info;
	int pet;
	int reset;
	int timeout;
} Flags;

Flags flags = {
    .run = 1,
    .cycles = -1,
    .pet = 1,
};

void
millisleep(long ms)
{
	struct timespec tp;

	tp.tv_sec = ms / 1000;
	tp.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&tp, NULL);
}

void
sighandle(int sig)
{
	flags.run = 0;
	(void)sig;
}

void
show(int fd)
{
	struct watchdog_info wi;
	int timeout;
	int status;
	int bootstatus;
	int timeleft;
	int pretimeout;

	memset(&wi, 0, sizeof(wi));
	timeout = -1;
	status = -1;
	bootstatus = -1;
	timeleft = -1;
	pretimeout = -1;

	ioctl(fd, WDIOC_GETSUPPORT, &wi);
	ioctl(fd, WDIOC_GETTIMEOUT, &timeout);
	ioctl(fd, WDIOC_GETSTATUS, &status);
	ioctl(fd, WDIOC_GETBOOTSTATUS, &bootstatus);
	ioctl(fd, WDIOC_GETTIMELEFT, &timeleft);
	ioctl(fd, WDIOC_GETPRETIMEOUT, &pretimeout);

	printf("Watchdog info:\n");
	printf("\tflags:       %#x\n", wi.flags);
	printf("\tversion:     %d\n", wi.version);
	printf("\tname:        %s\n", wi.name);
	printf("\ttimeout:     %d\n", timeout);
	printf("\tstatus:      %d\n", status);
	printf("\tboot status: %d\n", bootstatus);
	printf("\ttime left:   %d\n", timeleft);
	printf("\tpre-timeout: %d\n", pretimeout);
	printf("\n");
}

void
usage(void)
{
	fprintf(stderr, "usage: [options] <watchdog_device>\n");
	fprintf(stderr, "  -h   show this message\n");
	fprintf(stderr, "  -c   run for number of seconds (default: forever)\n");
	fprintf(stderr, "  -i   show device info\n");
	fprintf(stderr, "  -p   pet watchdog (default: %d)\n", flags.pet);
	fprintf(stderr, "  -r   force watchdog reset\n");
	fprintf(stderr, "  -t   set timeout\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	char data;
	int delay;
	int cycles;
	int option;
	int fd;
	int i, c, r;

	while ((c = getopt(argc, argv, "c:hip:rt:")) != -1) {
		switch (c) {
		case 'c':
			flags.cycles = atoi(optarg);
			break;

		case 'h':
			usage();
			break;

		case 'i':
			flags.info = 1;
			break;

		case 'p':
			flags.pet = atoi(optarg);
			break;

		case 'r':
			flags.reset = 1;
			break;

		case 't':
			flags.timeout = atoi(optarg);
			break;
		}
	}

	argc -= optind;
	argv += optind;
	if (argc < 1)
		usage();

	fd = open(argv[0], O_RDWR);
	if (fd < 0)
		err(1, "open");

	r = 0;
	if (flags.reset) {
		printf("Forcing reset\n");
		option = WDIOS_FORCERESET;
		if (ioctl(fd, WDIOC_SETOPTIONS, &option) < 0)
			err(1, "ioctl");
		
		sleep(3);
		printf("Failed to force reset\n");
		goto out;
	} else if (flags.timeout > 0) {
		printf("Setting timeout to %d\n", flags.timeout);
		r = ioctl(fd, WDIOC_SETTIMEOUT, &flags.timeout);
	} else if (flags.info) {
		show(fd);
		goto out;
	}

	if (r < 0)
		err(1, "ioctl");

	signal(SIGINT, sighandle);

	delay = 2;
	data = 0;
	cycles = flags.cycles;
	while (flags.run) {
		for (i = 0; i < delay; i++) {
			if (!flags.reset && flags.pet) {
				if (read(fd, &data, sizeof(data)) < 0)
					err(1, "Failed to pet watchdog");
			}

			millisleep(1000 / delay);
		}

		if (cycles > 0)
			cycles--;
		if (cycles == 0)
			break;
	}

out:
	close(fd);
	return 0;
}
