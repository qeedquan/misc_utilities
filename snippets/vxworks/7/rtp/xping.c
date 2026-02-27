#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <vxWorks.h>
#include <pingLib.h>

typedef struct {
	int packets;
	ulong_t options;
} Conf;

Conf conf = {
    .packets = 4,
};

void
usage(void)
{
	fprintf(stderr, "usage: [options] <host>\n");
	fprintf(stderr, "\t-d\tenable debugging\n");
	fprintf(stderr, "\t-h\tshow this message\n");
	fprintf(stderr, "\t-p\tsend number of packets (%d)\n", conf.packets);
	exit(2);
}

void
parseargs(int *argc, char ***argv)
{
	int c;

	while ((c = getopt(*argc, *argv, "h")) != -1) {
		switch (c) {
		case 'd':
			conf.options |= PING_OPT_DEBUG;
			break;

		case 'h':
			usage();
			break;

		case 'p':
			conf.packets = atoi(optarg);
			break;
		}
	}

	*argc -= optind;
	*argv += optind;
	
	if (*argc != 1)
		usage();
}

int
main(int argc, char *argv[])
{
	parseargs(&argc, &argv);
	if (ping(argv[0], conf.packets, conf.options) != OK)
		printf("Failed to ping\n");

	return 0;
}
