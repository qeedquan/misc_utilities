#include <gio/gio.h>
#include <glib.h>

typedef struct {
	const char *app;
	const char *title;
	const char *message;
	const char *icon;
	int priority;
} Option;

Option opt = {
	.app = "notification.test",
	.title = "Notification Test",
	.message = "This is an example notification.",
	.icon = "dialog-information",
	.priority = 0,
};

GOptionEntry opt_entries[] = {
	{ "title", 't', 0, G_OPTION_ARG_STRING, &opt.title, "Set Title", NULL },
	{ "message", 'm', 0, G_OPTION_ARG_STRING, &opt.message, "Set Message", NULL },
	{ "icon", 'i', 0, G_OPTION_ARG_STRING, &opt.icon, "Set Icon", NULL },
	{ "priority", 'p', 0, G_OPTION_ARG_INT, &opt.priority, "Set Priority", NULL },
};

int
main(int argc, char *argv[])
{
	GNotificationPriority priorities[] = {
		G_NOTIFICATION_PRIORITY_NORMAL,
		G_NOTIFICATION_PRIORITY_LOW,
		G_NOTIFICATION_PRIORITY_HIGH,
		G_NOTIFICATION_PRIORITY_URGENT,
	};

	GApplication *application;
	GNotification *notification;
	GOptionContext *context;
	GIcon *icon;
	GError *error;

	context = g_option_context_new("option parsing");
	g_option_context_add_main_entries(context, opt_entries, NULL);
	if (!g_option_context_parse(context, &argc, &argv, &error)) {
		g_print("option parsing failed: %s\n", error->message);
		exit(1);
	}

	g_assert(g_application_id_is_valid(opt.app));
	application = g_application_new(opt.app, G_APPLICATION_FLAGS_NONE);
	error = NULL;
	g_application_register(application, NULL, &error);
	g_assert_no_error(error);

	notification = g_notification_new(opt.title);
	g_notification_set_body(notification, opt.message);

	opt.priority = CLAMP(opt.priority, 0, (int)G_N_ELEMENTS(priorities) - 1);
	g_notification_set_priority(notification, priorities[opt.priority]);

	icon = g_themed_icon_new(opt.icon);
	g_notification_set_icon(notification, icon);
	g_application_send_notification(application, opt.app, notification);

	g_object_unref(icon);
	g_object_unref(notification);
	g_object_unref(application);

	return 0;
}
