#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <limits.h>
#include <getopt.h>
#include <systemd/sd-bus.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

typedef struct {
	char interface[PATH_MAX];
	int retries;
	int offset;
} Option;

typedef struct {
	char uuid[32][128];
	int blocked;
	int connected;
	int legacy_pairing;
	int paired;
	int services_resolved;
	int trusted;
	int16_t rssi;
	int16_t txpower;
	uint32_t class;
	char addr[64];
	char addrtype[64];
	char alias[64];
	char icon[64];
	char modalias[64];
	char name[64];
	char adapter[64];
} Prop;

enum {
	MAX_SERVICE = 256,
	MAX_CHARACTERISTIC = 256,
};

typedef struct {
	char path[PATH_MAX];
	char uuid[128];
} Characteristic;

typedef struct {
	char path[PATH_MAX];
	Characteristic characteristic[MAX_CHARACTERISTIC];
} Service;

typedef struct {
	int connected;
} Context;

Option opt = {
	.interface = "hci0",
	.retries = 5,
	.offset = 0,
};

Context ctx;

void
fatal(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(1);
}

void
usage(void)
{
	fprintf(stderr, "usage: [options] <bdaddr>\n");
	fprintf(stderr, "  -h    show this message\n");
	fprintf(stderr, "  -i    specify interface (default: %s)\n", opt.interface);
	fprintf(stderr, "  -o    specify offset (default: %d)\n", opt.offset);
	fprintf(stderr, "  -r    specify conection retries (default: %d)\n", opt.retries);
	exit(2);
}

void
parseargs(int *argc, char ***argv)
{
	int c;

	while ((c = getopt(*argc, *argv, "hi:o:r:")) != -1) {
		switch (c) {
		case 'h':
		default:
			usage();
			break;
		case 'i':
			snprintf(opt.interface, sizeof(opt.interface), "%s", optarg);
			break;
		case 'o':
			opt.offset = atoi(optarg);
			break;
		case 'r':
			opt.retries = atoi(optarg);
			break;
		}
	}

	*argc -= optind;
	*argv += optind;
	if (*argc < 1)
		usage();
}

char *
bd2db(const char *addr, char *bdaddr, size_t len)
{
	static const char *fmts[] = {
		"%02X-%02X-%02X-%02X-%02X-%02X",
		"%02X:%02X:%02X:%02X:%02X:%02X",
		"%02X_%02X_%02X_%02X_%02X_%02X",
	};

	size_t i;
	int b[6];

	for (i = 0; i < nelem(fmts); i++) {
		if (sscanf(addr, fmts[i], &b[0], &b[1], &b[2], &b[3], &b[4], &b[5]) == 6)
			break;
	}
	if (i == nelem(fmts))
		return NULL;

	snprintf(bdaddr, len, fmts[2], b[0], b[1], b[2], b[3], b[4], b[5]);
	return bdaddr;
}

void
discover(void)
{
}

void
dial(sd_bus *bus, const char *path, int retries)
{
	sd_bus_error err;
	sd_bus_message *m;
	int i, r;

	for (i = 0; i < retries; i++) {
		printf("trying to connect (attemp %d)\n", i);

		err = SD_BUS_ERROR_NULL;
		r = sd_bus_call_method(bus, "org.bluez", path, "org.bluez.Device1", "Connect", &err, &m, "");
		if (r == -EINPROGRESS) {
			printf("connect already in progress\n");
		} else if (r == -EWOULDBLOCK) {
			printf("waiting for connection (taking a while)\n");
			int i;
			for (i = 0; i < 100; i++) {
				r = sd_bus_process(bus, NULL);
				if (r < 0)
					fatal("failed to process bus: %s\n", strerror(-r));

				if (r > 0)
					continue;

				r = sd_bus_wait(bus, UINT64_MAX);
				if (r < 0)
					fatal("failed to wait on bus: %s\n", strerror(-r));
			}
		} else if (r < 0)
			fatal("failed to establish connection: %s", err.message);
		else if (r == 0)
			break;

		sd_bus_error_free(&err);
	}

	sd_bus_message_unref(m);
	sd_bus_error_free(&err);
}

int
readpropcb(sd_bus_message *m, const char *type, const char *key, void *ud)
{
	union {
		int i;
		unsigned u;
		int16_t i16;
		int32_t i32;
		int64_t i64;
		uint16_t u16;
		uint32_t u32;
		uint64_t u64;
		const char *s;
	} u;
	char **sv;
	Prop *p;
	size_t i;
	int r;

	memset(&u, 0, sizeof(u));
	p = ud;
	sv = NULL;
	r = sd_bus_message_read_basic(m, *type, &u);
	if (r < 0) {
		if (!strcmp(type, "as"))
			r = sd_bus_message_read_strv(m, &sv);
		if (r < 0)
			goto skip;
	}

	r = 1;
	if (!strcmp(key, "RSSI"))
		p->rssi = u.i16;
	else if (!strcmp(key, "UUIDs")) {
		for (i = 0; i < nelem(p->uuid) && sv[i]; i++)
			snprintf(p->uuid[i], sizeof(p->uuid[i]), "%s", sv[i]);
	} else if (!strcmp(key, "Name"))
		snprintf(p->name, sizeof(p->name), "%s", u.s);
	else if (!strcmp(key, "Alias"))
		snprintf(p->alias, sizeof(p->alias), "%s", u.s);
	else if (!strcmp(key, "Modalias"))
		snprintf(p->modalias, sizeof(p->modalias), "%s", u.s);
	else if (!strcmp(key, "Trusted"))
		p->trusted = u.i;
	else if (!strcmp(key, "Connected"))
		p->connected = u.i;
	else if (!strcmp(key, "Paired"))
		p->paired = u.i;
	else if (!strcmp(key, "LegacyPairing"))
		p->legacy_pairing = u.i;
	else if (!strcmp(key, "Blocked"))
		p->blocked = u.i;
	else if (!strcmp(key, "TxPower"))
		p->txpower = u.i16;
	else if (!strcmp(key, "Address"))
		snprintf(p->addr, sizeof(p->addr), "%s", u.s);
	else if (!strcmp(key, "AddressType"))
		snprintf(p->addrtype, sizeof(p->addrtype), "%s", u.s);
	else if (!strcmp(key, "ServicesResolved"))
		p->services_resolved = u.i;
	else if (!strcmp(key, "Adapter"))
		snprintf(p->adapter, sizeof(p->adapter), "%s", u.s);
	else if (!strcmp(key, "Class"))
		p->class = u.u32;
	else {
	skip:
		r = 0;
		printf("skipping property %s\n", key);
	}
	if (sv) {
		for (i = 0; sv[i]; i++)
			free(sv[i]);
	}
	free(sv);
	return r;
}

int
mapprop(sd_bus_message *m, int (*f)(sd_bus_message *, const char *, const char *, void *), void *ud)
{
	const char *key, *contents;
	int r;

	r = sd_bus_message_enter_container(m, SD_BUS_TYPE_ARRAY, "{sv}");
	if (r < 0)
		return r;

	while ((r = sd_bus_message_enter_container(m, SD_BUS_TYPE_DICT_ENTRY, "sv")) > 0) {
		r = sd_bus_message_read_basic(m, SD_BUS_TYPE_STRING, &key);
		if (r < 0)
			return r;

		r = sd_bus_message_peek_type(m, NULL, &contents);
		if (r < 0)
			return r;

		r = sd_bus_message_enter_container(m, SD_BUS_TYPE_VARIANT, contents);
		if (r < 0)
			return r;

		r = f(m, contents, key, ud);
		if (r < 0)
			return r;
		if (r == 0)
			sd_bus_message_skip(m, contents);

		r = sd_bus_message_exit_container(m);
		if (r < 0)
			return r;

		r = sd_bus_message_exit_container(m);
		if (r < 0)
			return r;
	}
	return 0;
}

int
devpropcb(sd_bus_message *m, void *u, sd_bus_error *err)
{
	printf("changed\n");
	return 1;

	(void)m;
	(void)u;
	(void)err;
}

int
readprop(sd_bus *bus, Prop *p, const char *path)
{
	sd_bus_error err;
	sd_bus_message *m;
	int r;

	memset(p, 0, sizeof(*p));

	printf("reading properties\n");
	err = SD_BUS_ERROR_NULL;
	r = sd_bus_call_method(bus, "org.bluez", path, "org.freedesktop.DBus.Properties", "GetAll", &err, &m, "s", "org.bluez.Device1");
	if (r < 0) {
		fprintf(stderr, "failed to read properties: %s\n", err.message);
		return r;
	}

	r = mapprop(m, readpropcb, p);
	if (r < 0) {
		fprintf(stderr, "failed to read properties: %s\n", strerror(-r));
		return r;
	}

	sd_bus_message_unref(m);
	sd_bus_error_free(&err);
	return 0;
}

void
dumpprop(Prop *p)
{
	size_t i;

	printf("Properties {\n");
	printf("\tUUID: [");
	for (i = 0; i < nelem(p->uuid) && p->uuid[i][0]; i++) {
		printf("%s", p->uuid[i]);
		if (i + 1 < nelem(p->uuid) && p->uuid[i + 1][0])
			printf(",");
	}
	printf("]\n");
	printf("\tName: %s\n", p->name);
	printf("\tAlias: %s\n", p->alias);
	printf("\tAddress: %s\n", p->addr);
	printf("\tAddress Type: %s\n", p->addrtype);
	printf("\tModalias: %s\n", p->modalias);
	printf("\tConnected: %d\n", p->connected);
	printf("\tPaired: %d\n", p->paired);
	printf("\tLegacy Pairing: %d\n", p->legacy_pairing);
	printf("\tServices Resolved: %d\n", p->services_resolved);
	printf("\tTrusted: %d\n", p->trusted);
	printf("\tBlocked: %d\n", p->blocked);
	printf("\tRSSI: %" PRIi16 "\n", p->rssi);
	printf("\tTX Power: %" PRIi16 "\n", p->txpower);
	printf("\tClass: %" PRIx32 "\n", p->class);
	printf("\tAdapter: %s\n", p->adapter);
	printf("}\n");
}

void
getservice(sd_bus *bus, const char *path, Service service[MAX_SERVICE])
{
	union {
		char *s;
	} u;
	sd_bus_message *m;
	sd_bus_error err;
	Service *s;
	Characteristic *c;
	size_t i, j;
	char fmt[PATH_MAX];
	int r, v[3];

	printf("reading services\n");
	err = SD_BUS_ERROR_NULL;
	m = NULL;
	r = sd_bus_call_method(bus, "org.bluez", "/", "org.freedesktop.DBus.ObjectManager", "GetManagedObjects", &err, &m, "");
	if (r < 0)
		fatal("failed to get managed objects: %s\n", err.message);

	printf("type signature %s\n", sd_bus_message_get_signature(m, true));

	r = sd_bus_message_enter_container(m, SD_BUS_TYPE_ARRAY, "{oa{sa{sv}}}");
	if (r < 0)
		goto error;

	memset(service, 0, sizeof(*service) * MAX_SERVICE);
	i = j = 0;
	while ((r = sd_bus_message_enter_container(m, SD_BUS_TYPE_DICT_ENTRY, "oa{sa{sv}}")) > 0) {
		r = sd_bus_message_read_basic(m, 'o', &u);
		if (r < 0)
			goto error;

		r = sd_bus_message_skip(m, "a{sa{sv}}");
		if (r < 0)
			goto error;

		r = sd_bus_message_exit_container(m);
		if (r < 0)
			goto error;

		snprintf(fmt, sizeof(fmt), "%s/service%%x/char%%x/desc%%x", path);
		r = sscanf(u.s, fmt, &v[0], &v[1], &v[2]);
		if (r == 1 && i < MAX_SERVICE) {
			s = &service[i++];
			j = 0;
			snprintf(s->path, sizeof(s->path), "%s", u.s);
		} else if (r == 2 && i > 0 && j < MAX_CHARACTERISTIC) {
			s = &service[i - 1];
			c = &s->characteristic[j++];
			snprintf(c->path, sizeof(c->path), "%s", u.s);
		}
	}

	if (0) {
	error:
		fatal("failed to read object message: %s\n", strerror(-r));
	}

	sd_bus_message_unref(m);
	sd_bus_error_free(&err);
}

void
dumpservice(sd_bus *bus, Service service[MAX_SERVICE])
{
	sd_bus_error err;
	sd_bus_message *m;
	Service *s;
	Characteristic *c;
	size_t i, j, k, n;
	const uint8_t *b;
	const void *p;
	int r;

	for (i = 0; i < MAX_SERVICE; i++) {
		s = &service[i];
		if (s->path[0] == '\0')
			break;
		printf("%s {\n", s->path);
		for (j = 0; j < MAX_CHARACTERISTIC; j++) {
			c = &s->characteristic[j];
			if (c->path[0] == '\0')
				break;
			printf("\t%s\n", c->path);
		}
		printf("}\n");
	}

	for (i = 0; i < MAX_SERVICE; i++) {
		s = &service[i];
		if (s->path[0] == '\0')
			break;
		for (j = 0; j < MAX_CHARACTERISTIC; j++) {
			c = &s->characteristic[j];
			if (c->path[0] == '\0')
				break;
			printf("reading service %s\n", c->path);

			err = SD_BUS_ERROR_NULL;
			m = NULL;
			r = sd_bus_call_method(bus, "org.bluez", c->path, "org.bluez.GattCharacteristic1", "ReadValue", &err, &m, "a{sv}", 1, "offset", "q", opt.offset);
			if (r < 0) {
				printf("error reading service: %s\n", err.message);
				goto next;
			}

			r = sd_bus_message_read_array(m, 'y', &p, &n);
			if (r < 0) {
				printf("error reading reply: %s\n", strerror(-r));
				goto next;
			}

			b = p;
			printf("[");
			for (k = 0; k < n; k++) {
				printf("%#x", b[k]);
				if (k + 1 < n)
					printf(" ");
			}
			printf("]\n");

		next:
			sd_bus_error_free(&err);
			sd_bus_message_unref(m);
		}
	}
}

void
dump(const char *interface, const char *addr)
{
	Service *service;
	sd_bus *bus;
	sd_bus_error err;
	char path[PATH_MAX], bdaddr[80];
	Prop prop;
	int r;

	err = SD_BUS_ERROR_NULL;
	if (!bd2db(addr, bdaddr, sizeof(bdaddr)))
		fatal("invalid bluetooth address format");

	r = sd_bus_default_system(&bus);
	if (r < 0)
		fatal("failed to connect to dbus: %s", strerror(-r));

	snprintf(path, sizeof(path), "/org/bluez/%s/dev_%s", interface, bdaddr);
	printf("trying to connect to dbus path %s\n", path);

	r = sd_bus_match_signal(bus, NULL, "org.bluez", path, "org.bluez.Device1", "PropertiesChanged", devpropcb, NULL);
	if (r < 0)
		fatal("failed to connect signal: %s\n", strerror(-r));

	r = sd_bus_get_property_trivial(bus, "org.bluez", path, "org.bluez.Device1", "Connected", &err, 'b', &ctx.connected);
	if (r == -EBADR) {
		printf("%s is not on the device list, attempting discovery\n", addr);
		discover();
	} else if (r < 0)
		fatal("dbus i/o error (%d): %s", -r, err.message);

	if (!ctx.connected)
		dial(bus, path, opt.retries);

	printf("connected to %s\n", addr);

	r = readprop(bus, &prop, path);
	if (r < 0)
		fatal("failed to read property: %s", strerror(-r));
	dumpprop(&prop);

	service = calloc(MAX_SERVICE, sizeof(*service));
	assert(service);
	getservice(bus, path, service);
	dumpservice(bus, service);

	free(service);
	sd_bus_error_free(&err);
	sd_bus_unref(bus);
}

int
main(int argc, char *argv[])
{
	parseargs(&argc, &argv);
	dump(opt.interface, argv[0]);
	return 0;
}
