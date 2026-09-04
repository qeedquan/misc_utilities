#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>
#include <err.h>
#include <modbus/modbus.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

typedef struct {
	char network[128];
	char interface[PATH_MAX];
	int timeout;
	int debug;
	int addr[0x100];
	uint16_t wrdata[0x10000];
	size_t wrlen;
} Option;

Option opt = {
	.network = "rtu",
	.interface = "/dev/ttyS0:115200:N:8:2",
	.timeout = 500,
	.debug = 1,
	.addr = { [MODBUS_BROADCAST_ADDRESS] = 1 },
};

void
swapi(int *x, int *y)
{
	int t;

	t = *x;
	*x = *y;
	*y = t;
}

void
usage(void)
{
	size_t i, c;

	fprintf(stderr, "usage: modbusrw [options] <reg> <size> ...\n");
	fprintf(stderr, "  -h               show this message\n");
	fprintf(stderr, "  -i <interface>   set interface (default: %s)\n", opt.interface);
	fprintf(stderr, "  -n <network>     set network (default: %s)\n", opt.network);
	fprintf(stderr, "  -q               turn off verbose output\n");
	fprintf(stderr, "  -s <addresses>   set addresses (default: [");
	for (i = c = 0; i < nelem(opt.addr); i++) {
		if (opt.addr[i]) {
			if (c++ > 0) {
				if (i > 10) {
					fprintf(stderr, "...");
					break;
				}
				fprintf(stderr, ",");
			}
			fprintf(stderr, "%zu", i);
		}
	}
	fprintf(stderr, "])\n");
	fprintf(stderr, "  -t <timeout>     set timeout (default: %d)\n", opt.timeout);
	fprintf(stderr, "  -w <data>        write data\n");
	fprintf(stderr, "\navailable networks: rtu, tcp\n");
	exit(2);
}

char *
xstrdup(const char *s)
{
	char *p;

	p = strdup(s);
	if (!p)
		abort();
	return p;
}

size_t
scanhex(const char *s, uint16_t *b, size_t n)
{
	const char *p;
	char *ep;
	size_t i;

	p = s;
	for (i = 0; i < n && *p; i++) {
		b[i] = strtoul(p, &ep, 0);
		p = ep;
	}
	return i;
}

void
scanlist(const char *s, int *a, size_t n)
{
	char *t, *p, *sp;
	int x, y;

	memset(a, 0, sizeof(*a) * n);
	p = xstrdup(s);
	while ((t = strtok_r(p, ",", &sp))) {
		if (sscanf(t, "%i-%i", &x, &y) != 2) {
			sscanf(t, "%i", &x);
			y = x;
		}
		if (x < 0 || y < 0)
			continue;
		if (x > y)
			swapi(&x, &y);
		for (; x <= y && (size_t)x <= n; x++)
			a[x] = 1;
		p = NULL;
	}
	free(p);
}

void
parseopt(Option *o, int *argc, char ***argv)
{
	char *env;
	int c;

	if ((env = getenv("MODBUS_NETWORK")))
		snprintf(o->network, sizeof(o->network), "%s", env);

	if ((env = getenv("MODBUS_INTERFACE")))
		snprintf(o->interface, sizeof(o->interface), "%s", env);

	if ((env = getenv("MODBUS_ADDR")))
		scanlist(env, o->addr, nelem(o->addr));

	while ((c = getopt(*argc, *argv, "hi:n:qs:t:w:")) != -1) {
		switch (c) {
		case 'h':
			usage();
			break;
		case 'i':
			snprintf(o->interface, sizeof(o->interface), "%s", optarg);
			break;
		case 'n':
			snprintf(o->network, sizeof(o->network), "%s", optarg);
			break;
		case 'q':
			o->debug = 0;
			break;
		case 's':
			scanlist(optarg, o->addr, nelem(o->addr));
			break;
		case 't':
			sscanf(optarg, "%i", &o->timeout);
			break;
		case 'w':
			o->wrlen = scanhex(optarg, o->wrdata, nelem(o->wrdata));
			break;
		}
	}
	*argc -= optind;
	*argv += optind;

	if (o->timeout < 0)
		errx(1, "invalid timeout");

	if ((o->wrlen && *argc < 1) || (!o->wrlen && *argc < 2))
		usage();
}

void
hexdump(void *b, size_t n)
{
	unsigned char *p;
	size_t i;

	p = b;
	for (i = 0; i < n; i++) {
		printf("%02X ", p[i]);
		if ((i & 15) == 15)
			printf("\n");
	}
	if ((i & 15) != 15)
		printf("\n");
}

void
mbread(modbus_t *mb, unsigned slave, unsigned reg, unsigned size)
{
	uint16_t *val;
	int nr;

	printf("read %u:%u:%u\n", slave, reg, size);
	val = calloc(size, sizeof(*val));
	if (!val)
		goto error;

	if (modbus_set_slave(mb, slave) < 0)
		goto error;

	nr = modbus_read_registers(mb, reg, size, val);
	if (nr < 0)
		goto error;

	printf("%u:%u:%u read %d bytes\n", slave, reg, size, nr * 2);
	hexdump(val, nr * 2);

	if (0) {
	error:
		printf("failed to read data at %u:%u:%u\n", slave, reg, size);
	}
	free(val);
}

void
mbwrite(modbus_t *mb, unsigned slave, unsigned reg, uint16_t *buf, unsigned len)
{
	int nr;

	if (modbus_set_slave(mb, slave) < 0)
		goto error;

	nr = modbus_write_registers(mb, reg, len, buf);
	if (nr < 0 || (unsigned)nr != len)
		goto error;

	if (0) {
	error:
		printf("failed to write data at %u:%u:%u\n", slave, reg, len);
	}
}

modbus_t *
mbdial(const char *network, const char *interface)
{
	char path[PATH_MAX], *p;
	int baudrate, databits, stopbits;
	char parity;
	modbus_t *mb;

	mb = NULL;
	snprintf(path, sizeof(path), "%s", interface);
	if (!strcasecmp(network, "rtu")) {
		if (!(p = strchr(path, ':')))
			return NULL;

		*p++ = '\0';
		if (sscanf(p, "%d:%c:%d:%d", &baudrate, &parity, &databits, &stopbits) != 4)
			return NULL;

		mb = modbus_new_rtu(path, baudrate, parity, databits, stopbits);
	} else if (!strcasecmp(network, "tcp")) {
		if (!(p = strrchr(path, ':')))
			return NULL;
		*p++ = '\0';

		mb = modbus_new_tcp_pi(path, p);
	}

	return mb;
}

int
main(int argc, char *argv[])
{
	modbus_t *mb;
	unsigned long reg, size;
	size_t slave;
	int nargs;

	parseopt(&opt, &argc, &argv);
	mb = mbdial(opt.network, opt.interface);
	if (!mb)
		errx(1, "failed to create modbus interface");

	modbus_set_debug(mb, opt.debug);

	if (modbus_connect(mb) < 0)
		errx(1, "failed to connect to modbus");

	if (modbus_set_response_timeout(mb, opt.timeout / 1000, (opt.timeout % 1000) * 1000) < 0)
		errx(1, "failed to set response timeout");

	nargs = (opt.wrlen) ? 1 : 2;
	for (; argc >= nargs; argc -= nargs) {
		reg = strtoul(argv[0], NULL, 0);
		size = (opt.wrlen) ? opt.wrlen : strtoul(argv[1], NULL, 0);
		argv += nargs;
		for (slave = 0; slave < nelem(opt.addr); slave++) {
			if (opt.addr[slave] == 0)
				continue;

			if (opt.wrlen)
				mbwrite(mb, slave, reg, opt.wrdata, size);
			else
				mbread(mb, slave, reg, size);
		}
	}
	modbus_close(mb);
	modbus_free(mb);
	return 0;
}
