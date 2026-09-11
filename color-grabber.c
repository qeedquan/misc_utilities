// a real time color grabber, it gets a color from the mouse position on the screen
// cc -o color-grabber color-grabber.c `pkg-config --cflags --libs gtk+-3.0`
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <stdlib.h>

static gboolean sample;
static gint mx, my;
static guint32 red, green, blue;
static gboolean ansiterm;

static guint32
get_pixel(gint x, gint y)
{
	GdkWindow *root;
	GdkPixbuf *pixbuf;
	guchar *pixels;
	guint32 r, g, b;

	root = gdk_get_default_root_window();
	pixbuf = gdk_pixbuf_get_from_window(root, x, y, 1, 1);

	if (gdk_pixbuf_get_colorspace(pixbuf) != GDK_COLORSPACE_RGB) {
		fprintf(stderr, "root window not rgb!\n");
		return 0;
	}

	if (gdk_pixbuf_get_bits_per_sample(pixbuf) != 8) {
		fprintf(stderr, "root window bits per sample not 8!\n");
		return 0;
	}

	pixels = gdk_pixbuf_get_pixels(pixbuf);
	r = pixels[0];
	g = pixels[1];
	b = pixels[2];
	g_object_unref(pixbuf);

	return r | (g << 8) | (b << 16);
}

static gboolean
draw(GtkWidget *widget, cairo_t *cr, gpointer userdata)
{
	char text[256];

	cairo_set_source_rgb(cr, red * 1.0 / 255, green * 1.0 / 255, blue * 1.0 / 255);
	cairo_paint(cr);

	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_move_to(cr, 16, 25);
	cairo_rectangle(cr, 0, 0, 1024, 50);
	cairo_fill(cr);

	cairo_set_source_rgb(cr, 1, 1, 1);
	cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, 20);
	cairo_move_to(cr, 16, 25);

	snprintf(text, sizeof text, "Sampling: %s (%d, %d) - (%d, %d, %d) (%f, %f, %f) 0xFF%02X%02X%02X",
	         sample ? "on" : "off", mx, my, red, green, blue,
	         red * 1.0 / 255, green * 1.0 / 255, blue * 1.0 / 255,
	         blue, green, red);
	cairo_show_text(cr, text);

	(void) widget;
	(void) userdata;

	return TRUE;
}

static gboolean
timer(GtkWidget *widget)
{
	GdkScreen *screen;
	GdkDisplay *display;
	GdkWindow *root;
	GdkSeat *seat;
	gint x, y;
	guint32 p, r, g, b;

	screen = gdk_screen_get_default();
	root = gdk_screen_get_root_window(screen);
	display = gdk_screen_get_display(screen);
	seat = gdk_display_get_default_seat(display);
	gdk_window_get_device_position(root, gdk_seat_get_pointer(seat), &x, &y, NULL);

	p = get_pixel(x, y);
	r = p & 0xFF;
	g = (p >> 8) & 0xFF;
	b = (p >> 16) & 0xFF;

	if (sample) {
		if ((r != red && g != green && b != blue) || (mx != x && my != y)) {
			printf("(%d, %d) - (%d, %d, %d) (%f, %f, %f) 0xFF%02X%02X%02X",
			       x, y, r, g, b, r * 1.0 / 255, g * 1.0 / 255, b * 1.0 / 255, b, g, r);
			if (ansiterm)
				printf("\t\x1b[48;2;%d;%d;%dm    \033[0m", r, g, b);
			printf("\n");
		}

		red = r;
		green = g;
		blue = b;

		mx = x;
		my = y;
	}

	gtk_widget_queue_draw(widget);

	return TRUE;
}

gboolean
keypress(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
	if (event->keyval == GDK_KEY_space) {
		sample = !sample;
		return TRUE;
	}
	if (event->keyval == GDK_KEY_Escape)
		exit(0);
	
	(void) widget;
	(void) data;

	return FALSE;
}

void
usage(void)
{
	fprintf(stderr, "usage: -[hm]\n");
	fprintf(stderr, "  -h     show this message\n");
	fprintf(stderr, "  -m     disable ansi color printing\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *area;
	int c;

	gtk_init(&argc, &argv);

	ansiterm = TRUE;
	while ((c = getopt(argc, argv, "mh")) != -1) {
		switch (c) {
		case 'm':
			ansiterm = FALSE;
			break;
		case 'h':
		default:
			usage();
			break;
		}
	}
	argc -= optind;
	argv += optind;

	sample = TRUE;
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	area = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), area);

	gtk_widget_add_events(area, GDK_BUTTON_PRESS_MASK);

	g_signal_connect(G_OBJECT(area), "draw", G_CALLBACK(draw), NULL);
	g_signal_connect(G_OBJECT(window), "destroy", gtk_main_quit, NULL);
	g_signal_connect(G_OBJECT(window), "key_press_event", G_CALLBACK(keypress), NULL);
	g_timeout_add(1000 / 60, (GSourceFunc)timer, window);

	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 1024, 256);
	gtk_window_set_title(GTK_WINDOW(window), "Color Grabber");

	gtk_widget_show_all(window);
	gtk_main();

	return 0;
}
