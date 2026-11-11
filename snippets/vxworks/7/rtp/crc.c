#include <vxWorks.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include <err.h>
#include <vxChecksum.h>

typedef unsigned long ulong;

typedef struct {
	int alg;
	ulong crc;
	ulong xorout;
} Flag;

enum {
	CRC7,
	CRC8,
	CRC16_CCITTF,
	CRC16_CCITT,
	CRC16,
	CRC32
};

Flag flag = {
    .alg = CRC32,
    .crc = 0xffffffff,
    .xorout = 0xffffffff,
};

int
getalg(const char *name)
{
	static const struct {
		int alg;
		char name[32];
	} tab[] = {
	    {CRC7, "crc7"},
	    {CRC8, "crc8"},
	    {CRC16_CCITTF, "crc16-ccittf"},
	    {CRC16_CCITT, "crc16-ccitt"},
	    {CRC16, "crc16"},
	    {CRC32, "crc32"},
	};

	size_t i;

	for (i = 0; i < NELEMENTS(tab); i++) {
		if (!strcmp(name, tab[i].name))
			return tab[i].alg;
	}
	return -1;
}

ulong
getmask(int alg)
{
	switch (alg) {
	case CRC7:
	case CRC8:
		return 0xff;
	case CRC16_CCITTF:
	case CRC16_CCITT:
	case CRC16:
		return 0xffff;
	case CRC32:
		return 0xffffffff;
	}
	return 0;
}

void
usage(void)
{
	fprintf(stderr, "usage: [options] <file> ...\n");
	fprintf(stderr, "  -a   use algorithm (default: %s)\n", "crc32");
	fprintf(stderr, "  -h   show this usage\n");
	fprintf(stderr, "  -i   init (default: %#lx)\n", flag.crc);
	fprintf(stderr, "  -x   xorout (default: %#lx)\n", flag.xorout);
	fprintf(stderr, "\nalgorithms: crc7 crc8 crc16-ccittf crc16-ccitt crc16 crc32\n");
	exit(2);
}

void
parseflags(int *argc, char ***argv)
{
	int c;

	while ((c = getopt(*argc, *argv, "a:hi:x:")) != -1) {
		switch (c) {
		case 'a':
			flag.alg = getalg(optarg);
			if (flag.alg < 0)
				errx(1, "invalid algorithm '%s' specified", optarg);
			break;
		case 'h':
			usage();
			break;
		case 'i':
			flag.crc = strtoul(optarg, NULL, 0);
			break;
		case 'x':
			flag.xorout = strtoul(optarg, NULL, 0);
			break;
		}
	}
	*argc -= optind;
	*argv += optind;
}

ulong
flush(int alg, ulong crc, ulong mask, void *buf, size_t len)
{
	switch (alg) {
	case CRC7:
		crc = vxCrc7(crc, buf, len);
		break;
	case CRC8:
		crc = vxCrc8(buf, len);
		break;
	case CRC16_CCITTF:
		crc = vxCrcCcittFalse(crc, buf, len);
		break;
	case CRC16_CCITT:
		crc = vxCrcCcitt(crc, buf, len);
		break;
	case CRC16:
		crc = vxCrc16(crc, buf, len);
		break;
	case CRC32:
		crc = vxCrc32(crc, buf, len);
		break;
	default:
		assert(0);
		break;
	}
	return crc & mask;
}

void
checksum(int fd, int alg, ulong crc, ulong xorout)
{
	uint8_t buf[32768];
	ssize_t len, nr;
	ulong mask;

	mask = getmask(alg);
	crc &= mask;
	for (;;) {
		len = 0;
		if (alg == CRC8)
			buf[len++] = crc;

		nr = read(fd, buf + len, sizeof(buf) - len);
		if (nr > 0)
			len += nr;

		if (len > 0)
			crc = flush(alg, crc, mask, buf, len);

		if (nr <= 0)
			break;
	}
	crc = (crc ^ xorout) & mask;

	printf("%lx\n", crc);
}

int
main(int argc, char *argv[])
{
	int fd;
	int i;

	parseflags(&argc, &argv);
	if (argc < 1)
		checksum(0, flag.alg, flag.crc, flag.xorout);
	else {
		for (i = 0; i < argc; i++) {
			fd = open(argv[i], O_RDONLY);
			if (fd < 0) {
				fprintf(stderr, "%s: %s\n", argv[i], strerror(errno));
				continue;
			}

			checksum(fd, flag.alg, flag.crc, flag.xorout);
			close(fd);
		}
	}

	return 0;
}
