// cc -o request-name request-name.c `pkg-config --cflags --libs dbus-1`
#include <stdio.h>
#include <unistd.h>
#include <err.h>
#include <dbus/dbus.h>

int
main(void)
{
	DBusConnection *conn;
	DBusError err;

	dbus_error_init(&err);
	conn = dbus_bus_get(DBUS_BUS_SESSION, &err);
	if (dbus_error_is_set(&err))
		errx(1, "%s", err.message);

	// usually a unique name goes in the form of :N.NN
	printf("%s\n", dbus_bus_get_unique_name(conn));

	// the dbus connection is maintained until client quits the program
	sleep(10);

	return 0;
}
