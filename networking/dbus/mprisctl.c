#include <gio/gio.h>
#include <inttypes.h>

void
iterate_dictionary(GVariant *dictionary)
{
	GVariantIter iter;
	GVariant *value;
	gchar *key;
	const gchar *type;

	g_variant_iter_init(&iter, dictionary);
	while (g_variant_iter_next(&iter, "{sv}", &key, &value)) {
		type = g_variant_get_type_string(value);
		g_print("Item '%s' has type '%s'\n", key, type);
		if (!strcmp(type, "s") || !strcmp(type, "o")) {
			g_print("%s\n", g_variant_get_string(value, NULL));
		} else if (!strcmp(type, "x")) {
			g_print("%jd\n", (intmax_t)g_variant_get_int64(value));
		}
		g_variant_unref(value);
		g_free(key);
	}
}

gchar **
find_mpris_services(gsize *len, GError **err)
{
	static const gchar *objpath = "org.mpris.MediaPlayer2.";

	GDBusProxy *proxy;
	GVariant *result;
	gchar **list, **services;
	gsize i, j, n;

	*len = 0;
	*err = NULL;
	proxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION,
	                                      G_DBUS_PROXY_FLAGS_NONE,
	                                      NULL,
	                                      "org.freedesktop.DBus",
	                                      "/org/freedesktop/DBus",
	                                      "org.freedesktop.DBus",
	                                      NULL,
	                                      err);
	if (*err != NULL)
		return NULL;

	result = g_dbus_proxy_call_sync(proxy,
	                                "ListNames",
	                                NULL,
	                                G_DBUS_CALL_FLAGS_NONE,
	                                -1,
	                                NULL,
	                                err);
	if (*err != NULL)
		return NULL;

	g_variant_get(result, "(^as)", &list);

	n = 0;
	while (list[n])
		n++;
	services = g_new0(gchar *, n);

	for (i = j = 0; i < n; i++) {
		if (!strncmp(list[i], objpath, strlen(objpath)))
			services[j++] = g_strdup(list[i]);
		g_free(list[i]);
	}
	*len = j;

	g_object_unref(proxy);
	g_variant_unref(result);
	g_free(list);
	return services;
}

void
free_strings0(gchar **str)
{
	gsize i;

	for (i = 0; str[i]; i++)
		g_free(str[i]);
	g_free(str);
}

void
parse_options(int *argc, char ***argv)
{
	GOptionContext *context;
	GError *error;

	context = g_option_context_new("option parsing");
	if (!g_option_context_parse(context, argc, argv, &error))
		g_critical("Option parsing failed: %s\n", error->message);
	g_option_context_free(context);
}

void
dump_services(gchar **services, gsize len)
{
	gsize i;

	g_print("MPRIS Services\n");
	for (i = 0; i < len; i++) {
		g_print("%s\n", services[i]);
	}
}

void
do_command(const gchar *service, gchar **args, gsize arglen)
{
	GDBusProxy *proxy;
	GError *err;
	GVariant *result;

	err = NULL;
	proxy = g_dbus_proxy_new_for_bus_sync(G_BUS_TYPE_SESSION,
	                                      G_DBUS_PROXY_FLAGS_NONE,
	                                      NULL,
	                                      service,
	                                      "/org/mpris/MediaPlayer2",
	                                      "org.mpris.MediaPlayer2.Player",
	                                      NULL,
	                                      &err);
	if (err != NULL)
		g_critical("failed to connect to service: %s", err->message);

	if (!strcasecmp(args[0], "show")) {
		result = g_dbus_proxy_get_cached_property(proxy, "Metadata");
		g_assert(result);
		iterate_dictionary(result);
	} else if (!strcasecmp(args[0], "play")) {
		g_dbus_proxy_call_sync(proxy,
		                       "Play",
		                       NULL,
		                       G_DBUS_CALL_FLAGS_NONE,
		                       -1,
		                       NULL,
		                       &err);
		g_assert_no_error(err);
	} else if (!strcasecmp(args[0], "playpause")) {
		g_dbus_proxy_call_sync(proxy,
		                       "PlayPause",
		                       NULL,
		                       G_DBUS_CALL_FLAGS_NONE,
		                       -1,
		                       NULL,
		                       &err);
		g_assert_no_error(err);
	} else if (!strcasecmp(args[0], "prev")) {
		g_dbus_proxy_call_sync(proxy,
		                       "Previous",
		                       NULL,
		                       G_DBUS_CALL_FLAGS_NONE,
		                       -1,
		                       NULL,
		                       &err);
		g_assert_no_error(err);
	} else if (!strcasecmp(args[0], "next")) {
		g_dbus_proxy_call_sync(proxy,
		                       "Next",
		                       NULL,
		                       G_DBUS_CALL_FLAGS_NONE,
		                       -1,
		                       NULL,
		                       &err);
		g_assert_no_error(err);
	} else if (!strcasecmp(args[0], "volume")) {
		result = g_dbus_proxy_get_cached_property(proxy, "Volume");
		g_assert(result);
		g_print("Volume %f\n", g_variant_get_double(result));
	} else {
		g_critical("unknown command '%s'", args[0]);
	}

	(void)arglen;
}

int
main(int argc, char *argv[])
{
	GError *err;
	gchar **services;
	gboolean matched;
	gsize i, len;
	gint ret;

	ret = 0;
	parse_options(&argc, &argv);
	services = find_mpris_services(&len, &err);
	if (err)
		g_critical("Failed to find mpris services: %s\n", err->message);

	if (len == 0) {
		g_print("No MPRIS services found\n");
		ret = 1;
		goto out;
	}

	if (argc >= 3) {
		matched = FALSE;
		for (i = 0; i < len; i++) {
			if (g_regex_match_simple(argv[1], services[i], 0, 0)) {
				do_command(services[i], argv + 2, argc - 2);
				matched = TRUE;
			}
		}
		if (!matched)
			g_critical("No matching service found");
	} else
		dump_services(services, len);

out:
	free_strings0(services);
	return ret;
}
