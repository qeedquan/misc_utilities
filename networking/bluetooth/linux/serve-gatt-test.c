#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <err.h>
#include <libgen.h>
#include <systemd/sd-bus.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

int gtchr_prop_get(sd_bus *bus, const char *path, const char *interface, const char *property, sd_bus_message *reply, void *userdata, sd_bus_error *err);
int gtchr_read_value(sd_bus_message *m, void *userdata, sd_bus_error *err);
int gtchr_write_value(sd_bus_message *m, void *userdata, sd_bus_error *err);

int gtchr_desc_prop_get(sd_bus *bus, const char *path, const char *interface, const char *property, sd_bus_message *reply, void *userdata, sd_bus_error *err);
int gtchr_desc_read_value(sd_bus_message *m, void *userdata, sd_bus_error *err);

int gtsvc_prop_get(sd_bus *bus, const char *path, const char *interface, const char *property, sd_bus_message *reply, void *userdata, sd_bus_error *err);

int gtadv_prop_get(sd_bus *bus, const char *path, const char *interface, const char *property, sd_bus_message *reply, void *userdata, sd_bus_error *err);

int gt_unsupported(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);
int gt_nop(sd_bus_message *m, void *userdata, sd_bus_error *ret_error);

#define IFACE_NAME "test.gatt.server"
#define BASE_UUID "B670003C-0079-465C-9BA7-000000000000"

enum {
	MAX_SVC = 8,
	MAX_CHR = 8,
	MAX_BUF = 512,
};

typedef struct {
	int type;
	int perm;
	union {
		int i;
		float f;
		char s[MAX_BUF];
		struct {
			char b[MAX_BUF];
			size_t n;
		};
	} value;
} gtchr;

typedef struct {
	gtchr *chr;
	size_t chrlen;
} gtsvc;

typedef struct {
	int advertise;
	int bustype;
	char interface[64];
	char uuid[128];
	size_t bufsz;
	size_t numsvc;
	size_t numchr;
} flags;

typedef struct {
	sd_bus *bus;
	sd_bus *sysbus;
	char uuid[128];

	gtsvc *svc;
	size_t svclen;
} context;

flags flg = {
	.advertise = 1,
	.bustype = 1,
	.interface = "hci0",
	.uuid = BASE_UUID,
	.bufsz = MAX_BUF,
	.numsvc = MAX_SVC,
	.numchr = MAX_CHR,
};

const sd_bus_vtable adv_vtable[] = {
	SD_BUS_VTABLE_START(0),
	SD_BUS_PROPERTY("Type", "s", gtadv_prop_get, 0, 0),
	SD_BUS_PROPERTY("ServiceUUIDs", "as", gtadv_prop_get, 0, 0),
	SD_BUS_PROPERTY("Includes", "as", gtadv_prop_get, 0, 0),
	SD_BUS_METHOD("Release", "", "", gt_nop, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_VTABLE_END
};

const sd_bus_vtable svc_vtable[] = {
	SD_BUS_VTABLE_START(0),
	SD_BUS_PROPERTY("UUID", "s", gtsvc_prop_get, 0, 0),
	SD_BUS_PROPERTY("Primary", "b", gtsvc_prop_get, 0, 0),
	SD_BUS_VTABLE_END,
};

const sd_bus_vtable chr_vtable[] = {
	SD_BUS_VTABLE_START(0),
	SD_BUS_METHOD("ReadValue", "a{sv}", "ay", gtchr_read_value, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("WriteValue", "aya{sv}", "", gtchr_write_value, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("StartNotify", "", "", gt_unsupported, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("StopNotify", "", "", gt_nop, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_PROPERTY("UUID", "s", gtchr_prop_get, 0, 0),
	SD_BUS_PROPERTY("Service", "o", gtchr_prop_get, 0, 0),
	SD_BUS_PROPERTY("Flags", "as", gtchr_prop_get, 0, 0),
	SD_BUS_VTABLE_END,
};

static const sd_bus_vtable chr_desc_vtable[] = {
	SD_BUS_VTABLE_START(0),
	SD_BUS_METHOD("ReadValue", "a{sv}", "ay", gtchr_desc_read_value, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("WriteValue", "aya{sv}", "", gt_nop, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_PROPERTY("UUID", "s", gtchr_desc_prop_get, 0, 0),
	SD_BUS_PROPERTY("Characteristic", "o", gtchr_desc_prop_get, 0, 0),
	SD_BUS_PROPERTY("Flags", "as", gtchr_desc_prop_get, 0, 0),
	SD_BUS_VTABLE_END
};

void *
xcalloc(size_t nmemb, size_t size)
{
	void *ptr;

	if (nmemb == 0)
		nmemb = 1;
	if (size == 0)
		size = 1;

	ptr = calloc(nmemb, size);
	assert(ptr);
	return ptr;
}

size_t
put4le(void *b, uint32_t v)
{
	uint8_t *p;

	p = b;
	p[0] = v & 0xff;
	p[1] = (v >> 8) & 0xff;
	p[2] = (v >> 16) & 0xff;
	p[3] = (v >> 24) & 0xff;
	return 4;
}

size_t
putfle(void *b, float v)
{
	memcpy(b, &v, sizeof(v));
	return sizeof(float);
}

uint32_t
get4le(void *b)
{
	uint8_t *p;

	p = b;
	return p[0] | p[1] << 8 | p[2] << 16 | p[3] << 24;
}

float
getfle(void *b)
{
	float f;

	memcpy(&f, b, sizeof(f));
	return f;
}

int
gt_get_offset(sd_bus_message *m)
{
	const char *s;
	int r;
	uint16_t off;

	off = 0;
	r = sd_bus_message_enter_container(m, SD_BUS_TYPE_ARRAY, "{sv}");
	if (r < 0)
		goto error;

	while ((r = sd_bus_message_enter_container(m, SD_BUS_TYPE_DICT_ENTRY, "sv")) > 0) {
		r = sd_bus_message_read(m, "s", &s);
		if (r < 0)
			goto error;

		if (!strcmp(s, "offset")) {
			r = sd_bus_message_read(m, "v", "q", &off);
			if (r < 0)
				goto error;
		} else {
			r = sd_bus_message_skip(m, "v");
			if (r < 0)
				goto error;
		}

		r = sd_bus_message_exit_container(m);
		if (r < 0)
			goto error;
	}

	r = sd_bus_message_exit_container(m);
	if (r < 0)
		goto error;

	return off;

error:
	printf("error reading offset: %s\n", strerror(-r));
	return r;
}

char *
gt_make_uuid(const char *base_uuid, char *buf, unsigned sn, unsigned cn)
{
	uintmax_t u[5] = { 0 };

	sscanf(base_uuid, "%jx-%jx-%jx-%jx-%jx", &u[0], &u[1], &u[2], &u[3], &u[4]);
	u[4] += (sn << 8) | (cn + 1);
	sprintf(buf, "%08jx-%04jx-%04jx-%04jx-%012jx", u[0], u[1], u[2], u[3], u[4]);
	return buf;
}

int
gt_unsupported(sd_bus_message *reply, void *userdata, sd_bus_error *err)
{
	printf("%s()\n", __func__);
	return sd_bus_error_set(err, "org.bluez.Error.NotSupported", "Not Supported");

	(void)reply;
	(void)userdata;
	(void)err;
}

int
gt_nop(sd_bus_message *reply, void *userdata, sd_bus_error *err)
{
	printf("%s()\n", __func__);
	return 0;

	(void)reply;
	(void)userdata;
	(void)err;
}

int
gtchr_read_value(sd_bus_message *reply, void *userdata, sd_bus_error *err)
{
	context *ctx;
	sd_bus_message *m;
	const gtchr *chr;
	char buf[512];
	unsigned n, len;
	int r, off;
	void **args;

	args = (void **)userdata;
	ctx = args[0];
	n = (unsigned)(uintptr_t)(args[1]);
	chr = &ctx->svc[(n >> 8) & 0xff].chr[n & 0xff];

	off = gt_get_offset(reply);
	printf("%s(service = %u, characteristic = %u, type %c, offset=%d)\n",
	       __func__, (n >> 8) & 0xff, n & 0xff, chr->type, off);

	switch (chr->type) {
	case 'i':
		len = put4le(buf, chr->value.i);
		break;
	case 'f':
		len = putfle(buf, chr->value.f);
		break;
	case 's':
		len = snprintf(buf, sizeof(buf), "%s", chr->value.s);
		break;
	case 'b':
		len = chr->value.n;
		memcpy(buf, chr->value.b, len);
		break;
	default:
		assert(0);
	}

	r = sd_bus_message_new_method_return(reply, &m);
	r |= sd_bus_message_append_array(m, 'y', buf, len);
	if (r < 0)
		return -EINVAL;

	r = sd_bus_send(NULL, m, NULL);
	sd_bus_message_unref(m);
	return r;

	(void)err;
}

int
gtchr_write_value(sd_bus_message *reply, void *userdata, sd_bus_error *err)
{
	size_t len;
	const void *ptr;
	void **args;
	unsigned n;
	int off;

	args = (void **)userdata;
	n = (unsigned)(uintptr_t)(args[1]);

	ptr = NULL;
	len = 0;
	sd_bus_message_read_array(reply, 'y', &ptr, &len);

	off = gt_get_offset(reply);
	printf("%s(service = %u, characteristic = %u, len %zu, off=%d)\n",
	       __func__, (n >> 8) & 0xff, n & 0xff, len, off);
	return sd_bus_reply_method_return(reply, "");

	(void)err;
}

int
gtchr_prop_get(sd_bus *bus, const char *path, const char *interface,
               const char *property, sd_bus_message *reply,
               void *userdata, sd_bus_error *err)
{
	context *ctx;
	const gtchr *chr;
	char lpath[PATH_MAX], buf[512];
	unsigned n;
	int r;
	void **args;

	args = (void **)userdata;
	ctx = args[0];
	n = (unsigned)(uintptr_t)(args[1]);
	chr = &ctx->svc[(n >> 8) & 0xff].chr[n & 0xff];

	printf("%s(path = %s, interface = %s, property = %s)\n", __func__, path, interface, property);
	if (!strcmp(property, "UUID")) {
		return sd_bus_message_append(reply, "s", gt_make_uuid(ctx->uuid, buf, (n >> 8), n & 0xff));
	} else if (!strcmp(property, "Service")) {
		snprintf(lpath, sizeof(lpath), "%s", path);
		dirname(lpath);
		return sd_bus_message_append(reply, "o", lpath);
	} else if (!strcmp(property, "Flags")) {
		r = sd_bus_message_open_container(reply, 'a', "s");
		if (chr->perm & 0x2)
			r |= sd_bus_message_append(reply, "s", "read");
		if (chr->perm & 0x4)
			r |= sd_bus_message_append(reply, "s", "write");
		r |= sd_bus_message_close_container(reply);
		return (r < 0) ? -EINVAL : 0;
	}
	return -ENOENT;

	(void)bus;
	(void)err;
}

int
gtchr_desc_read_value(sd_bus_message *reply, void *userdata, sd_bus_error *err)
{
	char buf[8];
	sd_bus_message *m;
	context *ctx;
	gtchr *chr;
	unsigned n, id;
	void **args;
	int r;

	args = (void **)userdata;
	ctx = args[0];
	n = (unsigned)(uintptr_t)(args[1]);
	id = (unsigned)(uintptr_t)(args[2]);
	chr = &ctx->svc[(n >> 8) & 0xff].chr[n & 0xff];

	printf("%s(service %u, characteristic = %u, id = %#x)\n", __func__, (n >> 8) & 0xff, n & 0xff, id);

	switch (id) {
	case 0x2901:
		strcpy(buf, "TEST");
		r = sd_bus_message_new_method_return(reply, &m);
		r |= sd_bus_message_append_array(m, 'y', buf, strlen(buf));
		if (r < 0)
			return -EINVAL;

		r = sd_bus_send(NULL, m, NULL);
		sd_bus_message_unref(m);
		return r;

	case 0x2904:
		switch (chr->type) {
		case 'i':
			buf[0] = 16;
			break;
		case 'f':
			buf[0] = 20;
			break;
		case 's':
			buf[0] = 25;
			break;
		case 'b':
			buf[0] = 27;
			break;
		default:
			assert(0);
			break;
		}

		r = sd_bus_message_new_method_return(reply, &m);
		r |= sd_bus_message_append_array(m, 'y', buf, 1);
		if (r < 0)
			return -EINVAL;

		r = sd_bus_send(NULL, m, NULL);
		sd_bus_message_unref(m);
		return r;

		break;

	default:
		assert(0);
		break;
	}

	return -ENOENT;

	(void)err;
}

int
gtchr_desc_prop_get(sd_bus *bus, const char *path, const char *interface,
                    const char *property, sd_bus_message *reply,
                    void *userdata, sd_bus_error *err)
{
	char rpath[PATH_MAX], buf[128];
	void **args;
	unsigned id;

	args = (void **)userdata;
	id = (int)((uintptr_t)args[2]);

	printf("%s(path = %s, interface = %s, property = %s, id=%#x)\n", __func__, path, interface, property, id);
	if (!strcmp(property, "UUID")) {
		snprintf(buf, sizeof(buf), "%X", id);
		return sd_bus_message_append(reply, "s", buf);
	} else if (!strcmp(property, "Characteristic")) {
		snprintf(rpath, sizeof(rpath), "%s", path);
		dirname(rpath);
		return sd_bus_message_append(reply, "o", rpath);
	} else if (!strcmp(property, "Flags")) {
		return sd_bus_message_append(reply, "as", 1, "read");
	}
	return -ENOENT;

	(void)bus;
	(void)err;
}

int
gtsvc_prop_get(sd_bus *bus, const char *path, const char *interface,
               const char *property, sd_bus_message *reply,
               void *userdata, sd_bus_error *err)
{
	context *ctx;
	char buf[128];
	unsigned sn;
	void **args;

	args = (void **)userdata;
	ctx = args[0];
	sn = (unsigned)(uintptr_t)(args[1]);
	printf("%s(path = %s, interface = %s, property = %s)\n", __func__, path, interface, property);
	if (!strcmp(property, "UUID")) {
		return sd_bus_message_append(reply, "s", gt_make_uuid(ctx->uuid, buf, sn, 0));
	} else if (!strcmp(property, "Primary")) {
		return sd_bus_message_append(reply, "b", 1);
	}
	return -ENOENT;

	(void)bus;
	(void)err;
}

int
gtadv_prop_get(sd_bus *bus, const char *path, const char *interface,
               const char *property, sd_bus_message *reply, void *userdata,
               sd_bus_error *err)
{
	printf("%s(path = %s, interface = %s, property = %s)\n", __func__, path, interface, property);
	if (!strcmp(property, "Type")) {
		return sd_bus_message_append(reply, "s", "peripheral");
	} else if (!strcmp(property, "ServiceUUIDs")) {
		return sd_bus_message_append(reply, "as", 1, BASE_UUID);
	} else if (!strcmp(property, "Includes")) {
		return sd_bus_message_append(reply, "as", 1, "local-name");
	}

	return -ENOENT;

	(void)bus;
	(void)userdata;
	(void)err;
}

int
on_register(sd_bus_message *m, void *userdata, sd_bus_error *err)
{
	if (sd_bus_error_is_set(err))
		printf("Error registering GATT application: %s\n", err->message);
	else
		printf("Registration of GATT application successful\n");
	return 1;

	(void)m;
	(void)userdata;
}

void
init_dbus(context *ctx, flags *flg)
{
	sd_bus *bus, *sysbus;
	int r;

	r = sd_bus_open_system(&sysbus);
	if (r < 0)
		errx(1, "Failed to connect to system bus: %s", strerror(-r));

	if (flg->bustype)
		bus = sysbus;
	else {
		r = sd_bus_default_user(&bus);
		if (r < 0)
			errx(1, "Failed to connect to user bus: %s", strerror(-r));
	}

	r = sd_bus_add_object_manager(bus, NULL, "/");
	if (r < 0)
		errx(1, "Failed to add object manager: %s", strerror(-r));

	r = sd_bus_request_name(bus, IFACE_NAME, 0);
	if (r < 0) {
		fprintf(stderr, "Failed to acquire service name: %s\n", strerror(-r));
		if (r == -EEXIST)
			exit(1);
	}

	ctx->bus = bus;
	ctx->sysbus = sysbus;
}

void
init_gatt(context *ctx, flags *flg)
{
	static const unsigned chr_desc_table[] = { 0x2901, 0x2904 };

	gtsvc *svc;
	gtchr *chr;
	sd_bus *bus, *sysbus;
	char path[PATH_MAX];
	unsigned i, j, k;
	int r;
	void **args;

	// bus and system bus has to match in order for register to work
	// we allow user bus to be made so we can debug it
	// being on system bus denies programs like d-feet to dump it without
	// some modification to flagsuration
	bus = ctx->bus;
	sysbus = ctx->sysbus;

	printf("Creating GATT Services\n");
	for (i = 0; i < ctx->svclen; i++) {
		svc = &ctx->svc[i];

		args = xcalloc(2, sizeof(*args));
		args[0] = ctx;
		args[1] = (void *)(uintptr_t)i;

		snprintf(path, sizeof(path), "/service%02X", i);
		printf("Registering %s\n", path);
		r = sd_bus_add_object_vtable(bus,
		                             NULL,
		                             path,
		                             "org.bluez.GattService1",
		                             svc_vtable,
		                             args);
		if (r < 0)
			errx(1, "Failed to setup gatt service vtable: %s", strerror(-r));

		for (j = 0; j < svc->chrlen; j++) {
			chr = &svc->chr[j];
			args = xcalloc(2, sizeof(*args));
			args[0] = ctx;
			args[1] = (void *)(uintptr_t)((i << 8) | j);

			switch (j & 3) {
			case 0:
				chr->type = 'i';
				chr->value.i = (i << 8) | j;
				chr->perm = 0x2;
				break;

			case 1:
				chr->type = 'f';
				chr->value.f = (i << 8) | j;
				chr->perm = 0x6;
				break;

			case 2:
				chr->type = 's';
				for (k = 0; k < flg->bufsz; k++)
					chr->value.s[k] = '0' + (k % 10);
				chr->value.s[k - 1] = '\0';
				chr->perm = 0x6;
				break;

			case 3:
				chr->type = 'b';
				chr->value.n = flg->bufsz;
				for (k = 0; k < chr->value.n; k++)
					chr->value.b[k] = 'A' + (k % 26);
				chr->perm = 0x6;
				break;
			}

			snprintf(path, sizeof(path), "/service%02X/char%04X", i, j);
			printf("Registering %s\n", path);

			r = sd_bus_add_object_vtable(bus,
			                             NULL,
			                             path,
			                             "org.bluez.GattCharacteristic1",
			                             chr_vtable,
			                             args);
			if (r < 0)
				errx(1, "Failed to setup gatt characteristic vtable: %s", strerror(-r));

			for (k = 0; k < nelem(chr_desc_table); k++) {
				args = xcalloc(3, sizeof(*args));
				args[0] = ctx;
				args[1] = (void *)(uintptr_t)((i << 8) | j);
				args[2] = (void *)(uintptr_t)chr_desc_table[k];

				snprintf(path, sizeof(path), "/service%02X/char%04X/desc%03X", i, j, k);
				printf("Registering %s %#x\n", path, chr_desc_table[k]);
				r = sd_bus_add_object_vtable(bus,
				                             NULL,
				                             path,
				                             "org.bluez.GattDescriptor1",
				                             chr_desc_vtable,
				                             args);
				if (r < 0)
					errx(1, "Failed to setup gatt characteristic descriptor vtable: %s", strerror(-r));
			}
		}
	}

	printf("Registering GATT Application (%zu services, %zu characteristics)\n", flg->numsvc, flg->numchr);
	snprintf(path, sizeof(path), "/org/bluez/%s", flg->interface);
	r = sd_bus_call_method_async(sysbus,
	                             NULL,
	                             "org.bluez",
	                             path,
	                             "org.bluez.GattManager1",
	                             "RegisterApplication",
	                             on_register, ctx,
	                             "oa{sv}", "/", 0);
	if (r < 0)
		printf("Failed to register GATT Application: %s\n", strerror(-r));

	if (flg->advertise) {
		printf("Creating advertisement interface\n");
		r = sd_bus_add_object_vtable(bus, NULL, "/advertisement",
		                             "org.bluez.LEAdvertisement1",
		                             adv_vtable, ctx);
		if (r < 0)
			errx(1, "Failed to create advertisement vtable: %s", strerror(-r));

		snprintf(path, sizeof(path), "/org/bluez/%s", flg->interface);
		r = sd_bus_call_method_async(bus, NULL, "org.bluez", path, "org.bluez.LEAdvertisingManager1",
		                             "RegisterAdvertisement", on_register, ctx,
		                             "oa{sv}", "/advertisement", 0);
		if (r < 0)
			errx(1, "Failed to create advertisement: %s", strerror(-r));
	}
}

void
usage(void)
{
	fprintf(stderr, "usage: [options]\n");
	fprintf(stderr, "  -c    set number of characteristics (default: %zu)\n", flg.numchr);
	fprintf(stderr, "  -d    enable debugging\n");
	fprintf(stderr, "  -h    show this message\n");
	fprintf(stderr, "  -i    set interface (default: %s)\n", flg.interface);
	fprintf(stderr, "  -q    don't advertise ble services\n");
	fprintf(stderr, "  -s    set number of services (default: %zu)\n", flg.numsvc);
	fprintf(stderr, "  -u    set base uuid (default: %s)\n", flg.uuid);
	fprintf(stderr, "  -y    set size of buffer (default: %zu)\n", flg.bufsz);
	exit(2);
}

void
parse_flags(int *argc, char ***argv)
{
	int c;

	while ((c = getopt(*argc, *argv, "hi:c:s:y:d:qu:")) != -1) {
		switch (c) {
		case 'h':
			usage();
			break;

		case 'i':
			snprintf(flg.interface, sizeof(flg.interface), "%s", optarg);
			break;

		case 'c':
			flg.numchr = atoi(optarg);
			break;

		case 's':
			flg.numsvc = atoi(optarg);
			break;

		case 'y':
			flg.bufsz = atoi(optarg);
			break;

		case 'd':
			flg.bustype = 0;
			break;

		case 'q':
			flg.advertise = 0;
			break;

		case 'u':
			snprintf(flg.uuid, sizeof(flg.uuid), "%s", optarg);
			break;
		}
	}
	*argc -= optind;
	*argv += optind;

	if (flg.numsvc > MAX_SVC)
		errx(1, "Too much services specified");
	if (flg.numchr > MAX_CHR)
		errx(1, "Too much characteristics specified");
	if (flg.bufsz > MAX_BUF)
		errx(1, "String length too long");
}

void
init_ctx(context *ctx, flags *flg)
{
	gtsvc *svc;
	size_t i;

	ctx->svclen = flg->numsvc;
	ctx->svc = xcalloc(ctx->svclen, sizeof(*ctx->svc));
	for (i = 0; i < ctx->svclen; i++) {
		svc = &ctx->svc[i];
		svc->chrlen = flg->numchr;
		svc->chr = xcalloc(svc->chrlen, sizeof(*svc->chr));
	}
	snprintf(ctx->uuid, sizeof(ctx->uuid), "%s", flg->uuid);
}

int
main(int argc, char *argv[])
{
	context ctx;
	sd_bus *bus;
	int r;

	parse_flags(&argc, &argv);
	init_ctx(&ctx, &flg);
	init_dbus(&ctx, &flg);
	init_gatt(&ctx, &flg);
	bus = ctx.bus;

	printf("Starting GATT Event Loop...\n");
	for (;;) {
		r = sd_bus_process(bus, NULL);
		if (r < 0)
			errx(1, "Failed to process bus: %s", strerror(-r));
		if (r > 0)
			continue;

		r = sd_bus_wait(bus, UINT64_MAX);
		if (r < 0)
			errx(1, "Failed to wait on bus: %s", strerror(-r));
	}

	sd_bus_unref(bus);

	return 0;
}
