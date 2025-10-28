#include <gdk/gdk.h>
#include <gdk/gdkx.h>

int
main(int argc, char *argv[])
{
	GdkWindow *window;
	GdkPixbuf *screenshot;
	const char *filename;
	GError *error;

	gdk_init(&argc, &argv);
	window = gdk_get_default_root_window();
	screenshot = gdk_pixbuf_get_from_drawable(NULL, window, NULL,
	                                          0, 0, 0, 0,
	                                          gdk_screen_width(),
	                                          gdk_screen_height());

	filename = "screenshot.png";
	if (argc > 1)
		filename = argv[1];

	error = NULL;
	gdk_pixbuf_save(screenshot, filename, "png", &error, NULL);
	if (error != NULL) {
		g_info("Unable to save screenshot: %s\n", error->message);
		return 1;
	}

	return 0;
}
