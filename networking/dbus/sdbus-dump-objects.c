#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
#include <systemd/sd-bus.h>

bool
bus_type_is_container(char c)
{
	static const char valid[] = {
		SD_BUS_TYPE_ARRAY,
		SD_BUS_TYPE_VARIANT,
		SD_BUS_TYPE_STRUCT,
		SD_BUS_TYPE_DICT_ENTRY
	};

	return !!memchr(valid, c, sizeof(valid));
}

int
print_object(sd_bus_message *m, FILE *f, bool needs_space)
{
	const char *contents;
	char type;
	int r;
	union {
		uint8_t u8;
		uint16_t u16;
		int16_t s16;
		uint32_t u32;
		int32_t s32;
		uint64_t u64;
		int64_t s64;
		double d64;
		const char *string;
		int i;
	} basic;

	contents = NULL;
	for (;;) {
		r = sd_bus_message_peek_type(m, &type, &contents);
		if (r < 0)
			return r;
		if (r == 0)
			return needs_space;

		if (bus_type_is_container(type) > 0) {
			r = sd_bus_message_enter_container(m, type, contents);
			if (r < 0)
				return r;

			if (type == SD_BUS_TYPE_ARRAY) {
				unsigned n = 0;

				/* count array entries */
				for (;;) {

					r = sd_bus_message_skip(m, contents);
					if (r < 0)
						return r;
					if (r == 0)
						break;

					n++;
				}

				r = sd_bus_message_rewind(m, false);
				if (r < 0)
					return r;

				if (needs_space)
					fprintf(f, " ");

				fprintf(f, "%u", n);
				needs_space = true;

			} else if (type == SD_BUS_TYPE_VARIANT) {
				if (needs_space)
					fprintf(f, " ");

				fprintf(f, "%s", contents);
				needs_space = true;
			}

			r = print_object(m, f, needs_space);
			if (r < 0)
				return r;

			needs_space = r > 0;

			r = sd_bus_message_exit_container(m);
			if (r < 0)
				return r;

			continue;
		}

		r = sd_bus_message_read_basic(m, type, &basic);
		if (r < 0)
			return r;

		if (needs_space)
			fprintf(f, " ");

		switch (type) {
		case SD_BUS_TYPE_BYTE:
			fprintf(f, "%u", basic.u8);
			break;

		case SD_BUS_TYPE_BOOLEAN:
			fprintf(f, "%s", (basic.i) ? "true" : "false");
			break;

		case SD_BUS_TYPE_INT16:
			fprintf(f, "%i", basic.s16);
			break;

		case SD_BUS_TYPE_UINT16:
			fprintf(f, "%u", basic.u16);
			break;

		case SD_BUS_TYPE_INT32:
			fprintf(f, "%i", basic.s32);
			break;

		case SD_BUS_TYPE_UINT32:
			fprintf(f, "%u", basic.u32);
			break;

		case SD_BUS_TYPE_INT64:
			fprintf(f, "%" PRIi64, basic.s64);
			break;

		case SD_BUS_TYPE_UINT64:
			fprintf(f, "%" PRIu64, basic.u64);
			break;

		case SD_BUS_TYPE_DOUBLE:
			fprintf(f, "%g", basic.d64);
			break;

		case SD_BUS_TYPE_STRING:
		case SD_BUS_TYPE_OBJECT_PATH:
		case SD_BUS_TYPE_SIGNATURE: {
			char *b = NULL;

			b = strdup(basic.string);
			if (!b)
				return -ENOMEM;

			fprintf(f, "\"%s\"", b);
			free(b);
			break;
		}

		case SD_BUS_TYPE_UNIX_FD:
			fprintf(f, "%i", basic.i);
			break;

		default:
			assert(0);
		}

		needs_space = true;
	}

	return 0;
}

void
get_object(sd_bus *bus, const char *dest, const char *path, const char *interface, const char *method)
{
	sd_bus_error err;
	sd_bus_message *m;
	int r;

	err = SD_BUS_ERROR_NULL;
	m = NULL;
	r = sd_bus_call_method(bus, dest, path, interface, method, &err, &m, "");
	if (r < 0)
		goto error;

	printf("type: %s\n", sd_bus_message_get_signature(m, true));
	print_object(m, stdout, false);
	printf("\n\n\n");

	if (0) {
	error:
		printf("%s\n", strerror(-r));
	}
}

int
main(void)
{
	sd_bus *bus;
	int r;

	bus = NULL;
	r = sd_bus_open_system(&bus);
	if (r < 0)
		errx(1, "Failed to connect to system bus: %s", strerror(-r));

	get_object(bus,
	           "org.freedesktop.NetworkManager",
	           "/org/freedesktop",
	           "org.freedesktop.DBus.ObjectManager",
	           "GetManagedObjects");
	get_object(bus,
	           "org.bluez",
	           "/",
	           "org.freedesktop.DBus.ObjectManager",
	           "GetManagedObjects");

	return 0;
}