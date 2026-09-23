// http://0pointer.net/blog/the-new-sd-bus-api-of-systemd.html
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <systemd/sd-bus.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

static struct {
	double writable;
} ctx;

static int
method_add(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
	int64_t x, y;
	int r;

	(void)userdata;
	(void)ret_error;

	r = sd_bus_message_read(m, "xx", &x, &y);
	if (r < 0) {
		fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-r));
		return r;
	}

	return sd_bus_reply_method_return(m, "x", x + y);
}

static int
method_sub(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
	int64_t x, y;
	int r;

	(void)userdata;
	(void)ret_error;

	r = sd_bus_message_read(m, "xx", &x, &y);
	if (r < 0) {
		fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-r));
		return r;
	}

	return sd_bus_reply_method_return(m, "x", x - y);
}

static int
method_multiply(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
	int64_t x, y;
	int r;

	(void)userdata;
	(void)ret_error;

	r = sd_bus_message_read(m, "xx", &x, &y);
	if (r < 0) {
		fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-r));
		return r;
	}

	return sd_bus_reply_method_return(m, "x", x * y);
}

static int
method_divide(sd_bus_message *m, void *userdata, sd_bus_error *ret_error)
{
	int64_t x, y;
	int r;

	(void)userdata;
	(void)ret_error;

	r = sd_bus_message_read(m, "xx", &x, &y);
	if (r < 0) {
		fprintf(stderr, "Failed to parse parameters: %s\n", strerror(-r));
		return r;
	}

	if (y == 0) {
		sd_bus_error_set_const(ret_error, "sdbus.test.Calculator.DivisionByZero", "Sorry, can't allow division by zero.");
		return -EINVAL;
	}

	return sd_bus_reply_method_return(m, "x", x / y);
}

static int
property_path_read(sd_bus *bus, const char *path, const char *interface, const char *property, sd_bus_message *reply, void *userdata, sd_bus_error *error)
{
	printf("property_path_read(path = %s, interface = %s, property = %s)\n", path, interface, property);
	return sd_bus_message_append(reply, "d", 1.0);

	(void)bus;
	(void)userdata;
	(void)error;
}

static int
property_read(sd_bus *bus, const char *path, const char *interface, const char *property, sd_bus_message *reply, void *userdata, sd_bus_error *error)
{
	char buf[80];
	int i, r;

	printf("%s(property = %s, signature = %s)\n", __func__, property, sd_bus_message_get_signature(reply, true));
	if (!strcmp(property, "Pi"))
		return sd_bus_message_append(reply, "d", M_PI);
	if (!strcmp(property, "Tau"))
		return sd_bus_message_append(reply, "d", 2 * M_PI);
	if (!strcmp(property, "Writable"))
		return sd_bus_message_append(reply, "d", ctx.writable);
	if (!strcmp(property, "ListOfStuff"))
		return sd_bus_message_append(reply, "a{sv}", 5,
		                             "Stuff1", "i", 1,
		                             "Stuff2", "q", 453,
		                             "Stuff3", "d", 3.562,
		                             "Stuff4", "i", 4,
		                             "Stuff5", "s", "Blah");

	if (!strcmp(property, "DynamicListOfStuff")) {
		r = sd_bus_message_open_container(reply, 'a', "{sv}");
		if (r < 0)
			return r;
		for (i = 0; i < 16; i++) {
			r = sd_bus_message_open_container(reply, SD_BUS_TYPE_DICT_ENTRY, "sv");
			if (r < 0)
				return r;

			snprintf(buf, sizeof(buf), "Prop%d", i);
			r = sd_bus_message_append(reply, "sv", buf, "i", i);
			if (r < 0)
				return r;

			r = sd_bus_message_close_container(reply);
			if (r < 0)
				return r;
		}
		r = sd_bus_message_close_container(reply);
		if (r < 0)
			return r;

		return 0;
	}

	return -EINVAL;

	(void)bus;
	(void)path;
	(void)interface;
	(void)userdata;
	(void)error;
}

static int
property_write(sd_bus *bus, const char *path, const char *interface, const char *property, sd_bus_message *value, void *userdata, sd_bus_error *error)
{
	double v;
	int r;

	printf("%s(property = %s)\n", __func__, property);
	if (!strcmp(property, "Writable")) {
		r = sd_bus_message_read(value, "d", &v);
		if (r < 0)
			return -r;

		ctx.writable = v;
		return 0;
	}
	return -EINVAL;

	(void)bus;
	(void)path;
	(void)interface;
	(void)userdata;
	(void)error;
}

static const sd_bus_vtable calculator_vtable[] = {
	SD_BUS_VTABLE_START(0),
	SD_BUS_METHOD("Add", "xx", "x", method_add, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("Subtract", "xx", "x", method_sub, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("Multiply", "xx", "x", method_multiply, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_METHOD("Divide", "xx", "x", method_divide, SD_BUS_VTABLE_UNPRIVILEGED),
	SD_BUS_PROPERTY("Pi", "d", property_read, 0, 0),
	SD_BUS_PROPERTY("Tau", "d", property_read, 0, 0),
	SD_BUS_PROPERTY("ListOfStuff", "a{sv}", property_read, 0, 0),
	SD_BUS_PROPERTY("DynamicListOfStuff", "a{sv}", property_read, 0, 0),
	SD_BUS_WRITABLE_PROPERTY("Writable", "d", property_read, property_write, 0, 0),
	SD_BUS_VTABLE_END
};

static void
strv_free(char **s)
{
	size_t i;

	if (!s)
		return;

	for (i = 0; s[i]; i++)
		free(s[i]);
	free(s);
}

static int
enumerate(sd_bus *bus, const char *path, void *ud, char ***nodes, sd_bus_error *err)
{
	static const char *tab1[] = {
		"/",
		"/Path0",
		"/Path1",
		"/Path2",
		"/Path3",
		"/Path4",
		"/Path5",
		"/Path6",
		"/Path7",
	};

	static const char *tab2[] = {
		"/sdbus/test/Calculator",
	};

	const char **tab;
	size_t tabsz;
	char **n;
	size_t i;

	if (!path)
		return 0;

	printf("enumerate(path = %s)\n", path);
	if (!strcmp(path, "/")) {
		tab = tab1;
		tabsz = nelem(tab1);
	} else {
		tab = tab2;
		tabsz = nelem(tab2);
	}

	n = calloc(tabsz + 1, sizeof(*n));
	if (!n)
		goto error;

	for (i = 0; i < tabsz; i++) {
		n[i] = strdup(tab[i]);
		if (!n[i])
			goto error;
	}

	*nodes = n;
	return 1;

error:
	strv_free(n);
	return -ENOMEM;

	(void)bus;
	(void)ud;
	(void)err;
}

int
main(void)
{
	sd_bus_vtable vtable[8];
	char path[80];
	sd_bus_slot *slot = NULL;
	sd_bus *bus = NULL;
	int i, r;

	r = sd_bus_open_user(&bus);
	if (r < 0) {
		fprintf(stderr, "Failed to connect to system bus: %s\n", strerror(-r));
		goto finish;
	}

	r = sd_bus_add_object_vtable(bus,
	                             &slot,
	                             "/sdbus/test/Calculator",
	                             "sdbus.test.Calculator",
	                             calculator_vtable,
	                             NULL);
	if (r < 0) {
		fprintf(stderr, "Failed to setup vtable: %s\n", strerror(-r));
		goto finish;
	}

	r = sd_bus_request_name(bus, "sdbus.test", 0);
	if (r < 0) {
		fprintf(stderr, "Failed to acquire service name: %s\n", strerror(-r));
		goto finish;
	}

	// this handles enumeration when object manager method gets called
	r = sd_bus_add_node_enumerator(bus, NULL, "/sdbus/test/Calculator", enumerate, NULL);
	if (r < 0) {
		fprintf(stderr, "Failed to add enumerator: %s\n", strerror(-r));
		goto finish;
	}

	r = sd_bus_add_object_manager(bus, NULL, "/sdbus/test/Calculator");
	if (r < 0) {
		fprintf(stderr, "Failed to add object manager: %s\n", strerror(-r));
		goto finish;
	}

	r = sd_bus_add_node_enumerator(bus, NULL, "/", enumerate, NULL);
	if (r < 0) {
		fprintf(stderr, "Failed to add enumerator: %s\n", strerror(-r));
		goto finish;
	}

	r = sd_bus_add_object_manager(bus, NULL, "/");
	if (r < 0) {
		fprintf(stderr, "Failed to add object manager: %s\n", strerror(-r));
		goto finish;
	}

	// add some random paths
	// this is a way to add dynamic handlers
	for (i = 0; i < 8; i++) {
		snprintf(path, sizeof(path), "GetPath%d", i);
		vtable[0] = (sd_bus_vtable)SD_BUS_VTABLE_START(0);
		vtable[1] = (sd_bus_vtable)SD_BUS_PROPERTY("GetPath", "d", property_path_read, 0, 0),
		vtable[2] = (sd_bus_vtable)SD_BUS_VTABLE_END;

		snprintf(path, sizeof(path), "/Path%d", i);
		r = sd_bus_add_object_vtable(bus,
		                             &slot,
		                             path,
		                             "sdbus.path",
		                             vtable,
		                             NULL);
		if (r < 0) {
			fprintf(stderr, "Failed to setup vtable: %s\n", strerror(-r));
			goto finish;
		}
	}

	for (;;) {
		r = sd_bus_process(bus, NULL);
		if (r < 0) {
			fprintf(stderr, "Failed to process bus: %s\n", strerror(-r));
			goto finish;
		}
		if (r > 0)
			continue;

		r = sd_bus_wait(bus, (uint64_t)-1);
		if (r < 0) {
			fprintf(stderr, "Failed to wait on bus: %s\n", strerror(-r));
			goto finish;
		}
	}

finish:
	sd_bus_slot_unref(slot);
	sd_bus_unref(bus);

	return r < 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
