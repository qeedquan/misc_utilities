#include <stdio.h>
#include <gio/gio.h>

void
dump_services(const char *busstr, GBusType bustype)
{
	GDBusProxy *proxy;
	GVariant *id, *names;
	GError *err;
	gchar *str_id, **str_names;
	size_t i;

	err = NULL;
	proxy = g_dbus_proxy_new_for_bus_sync(bustype,
	                                      G_DBUS_PROXY_FLAGS_NONE,
	                                      NULL,
	                                      "org.freedesktop.DBus",
	                                      "/org/freedesktop/DBus",
	                                      "org.freedesktop.DBus",
	                                      NULL,
	                                      &err);

	g_assert(proxy);
	g_assert_no_error(err);

	id = g_dbus_proxy_call_sync(proxy,
	                            "GetId",
	                            NULL,
	                            G_DBUS_CALL_FLAGS_NONE,
	                            -1,
	                            NULL,
	                            &err);
	g_assert(id);
	g_assert_no_error(err);

	names = g_dbus_proxy_call_sync(proxy,
	                               "ListNames",
	                               NULL,
	                               G_DBUS_CALL_FLAGS_NONE,
	                               -1,
	                               NULL,
	                               &err);
	g_assert(names);
	g_assert_no_error(err);

	g_variant_get(id, "(s)", &str_id);
	g_variant_get(names, "(^as)", &str_names);
	g_print("%s id %s\n", busstr, str_id);
	for (i = 0; str_names[i]; i++) {
		g_print("\t%s\n", str_names[i]);
		g_free(str_names[i]);
	}
	g_print("\n");

	g_object_unref(proxy);
	g_variant_unref(id);
	g_free(str_id);
	g_free(str_names);
}

int
main(void)
{
	dump_services("system", G_BUS_TYPE_SYSTEM);
	dump_services("session", G_BUS_TYPE_SESSION);
	return 0;
}
