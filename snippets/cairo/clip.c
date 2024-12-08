#define _GNU_SOURCE
#include <stdio.h>
#include <math.h>
#include <cairo.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))

void
clip(const char *in, const char *out, int center)
{
	cairo_surface_t *image, *canvas;
	cairo_t *ci, *cr;
	double x, y, r;
	int w, h;

	canvas = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1024, 1024);
	cr = cairo_create(canvas);

	image = NULL;
	if (in)
		image = cairo_image_surface_create_from_png(in);
	if (!image) {
		image = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1024, 1024);
		ci = cairo_create(image);
		cairo_set_source_rgb(ci, 0.3, 0.4, 0.5);
		cairo_paint(ci);
		cairo_destroy(ci);
	}

	w = cairo_image_surface_get_width(image);
	h = cairo_image_surface_get_height(image);
	cairo_set_source_surface(cr, image, 0, 0);

	if (!center) {
		r = 10;
		cairo_set_source_surface(cr, image, 0, 0);
		for (y = 0; y < h; y += r * 2) {
			for (x = 0; x < w; x += r * 2) {
				cairo_arc(cr, x, y, r, 0, 2 * M_PI);
				cairo_fill(cr);
			}
		}
	} else {
		cairo_arc(cr, w / 2, h / 2, min(w / 2, h / 2), 0, 2 * M_PI);
		cairo_clip(cr);
		cairo_paint(cr);
	}

	cairo_surface_write_to_png(canvas, out);

	cairo_destroy(cr);
	cairo_surface_destroy(image);
	cairo_surface_destroy(canvas);
}

int
main(int argc, char *argv[])
{
	const char *name;

	name = (argc >= 2) ? argv[1] : NULL;
	clip(name, "clip_not_center.png", 0);
	clip(name, "clip_center.png", 1);

	return 0;
}
