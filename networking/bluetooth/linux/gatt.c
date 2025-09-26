#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>

static struct {
	GDBusConnection *bus;
	GDBusObjectManager *bluez;
	GMainLoop *loop;
} ctx;

void
on_acquired(GDBusConnection *conn, const char *name, gpointer user_data)
{
	g_print("%s(name = %s)\n", __func__, name);
	(void)conn;
	(void)user_data;
}

void
on_lost(GDBusConnection *conn, const char *name, gpointer user_data)
{
	g_print("%s(name = %s)\n", __func__, name);
	g_object_unref(ctx.bus);
	g_main_loop_quit(ctx.loop);
	(void)conn;
	(void)user_data;
}

void
create_dbus_interface(void)
{
	GMainContext *context;
	GMainLoop *loop;
	GDBusConnection *conn;
	GBusType bustype;
	GError *err;

	err = NULL;
	bustype = G_BUS_TYPE_SYSTEM;
	conn = g_bus_get_sync(bustype, NULL, &err);
	g_assert(conn);
	g_assert_no_error(err);

	// need a context before we can create a bus
	context = g_main_context_new();
	g_main_context_push_thread_default(context);
	loop = g_main_loop_new(context, FALSE);

	g_bus_own_name(
	    bustype,
	    "com.bluetooth.test",
	    G_BUS_NAME_OWNER_FLAGS_NONE,
	    NULL,
	    on_acquired,
	    on_lost,
	    NULL,
	    NULL);

	ctx.bus = conn;
	ctx.loop = loop;
}

void
create_bluez_object_manager(void)
{
	GDBusObjectManager *manager;
	GError *err;

	// a proxy object points to a dbus remote object
	// in this case we want the object representing bluez
	// interface for bluetooth management
	err = NULL;
	manager = g_dbus_object_manager_client_new_sync(
	    ctx.bus,
	    G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_NONE,
	    "org.bluez",
	    "/",
	    NULL,
	    NULL,
	    NULL,
	    NULL,
	    &err);
	g_assert_no_error(err);

	ctx.bluez = manager;
}

void
create_gatt_interface(void)
{
	GDBusObject *adapter;
	GDBusProxy *gatt, *intf, *intfprop;
	GList *objs;
	const gchar *path;
	guint i;

	objs = g_dbus_object_manager_get_objects(ctx.bluez);
	g_assert(objs);

	// bluetooth daemon should be running right now providing us these
	// interfaces which we query via dbus
	for (i = 0; i < g_list_length(objs); i++) {
		adapter = G_DBUS_OBJECT(g_list_nth_data(objs, i));
		if (adapter == NULL)
			continue;

		gatt = G_DBUS_PROXY(g_dbus_object_get_interface(adapter, "org.bluez.GattManager1"));
		if (gatt == NULL)
			continue;

		intf = G_DBUS_PROXY(g_dbus_object_get_interface(adapter, "org.bluez.Adapter1"));
		if (intf == NULL)
			continue;

		intfprop = G_DBUS_PROXY(g_dbus_object_get_interface(adapter, "org.freedesktop.DBus.Properties"));
		if (intfprop == NULL)
			continue;

		path = g_dbus_proxy_get_object_path(gatt);
		break;
	}

	g_assert(adapter && gatt && intf && intfprop);
	g_print("Created GATT interface path %s\n", path);
}

int
main(void)
{
	create_dbus_interface();
	create_bluez_object_manager();
	create_gatt_interface();
	g_main_loop_run(ctx.loop);
	return 0;
}
