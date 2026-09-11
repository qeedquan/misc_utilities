#include <gio/gio.h>

int
main(void)
{
	gchar *guid;

	guid = g_dbus_generate_guid();
	g_print("%s\n", guid);
	g_free(guid);

	return 0;
}
