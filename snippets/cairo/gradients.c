#define _GNU_SOURCE
#include <math.h>
#include <cairo.h>

enum {
	WIDTH = 1024,
	HEIGHT = 1024,
};

void
linear_gradient_1(cairo_t *cr)
{
	cairo_pattern_t *pat;
	double t;
	int i;

	pat = cairo_pattern_create_linear(0.0, 0.0, WIDTH, HEIGHT);
	i = 1;
	for (t = 0; t < 1.0; t += 0.1) {
		if (i & 1)
			cairo_pattern_add_color_stop_rgb(pat, t, 0, 0, 0);
		else
			cairo_pattern_add_color_stop_rgb(pat, t, 1, 0, 0);
		i++;
	}

	cairo_rectangle(cr, 0, 0, WIDTH, HEIGHT);
	cairo_set_source(cr, pat);
	cairo_fill(cr);

	cairo_pattern_destroy(pat);
}

void
linear_gradient_2(cairo_t *cr)
{
	cairo_pattern_t *pat;
	double t;
	int i;

	pat = cairo_pattern_create_linear(0, 0, WIDTH, 0);
	i = 1;
	for (t = 0; t < 1; t += 0.025) {
		if (!(i & 1))
			cairo_pattern_add_color_stop_rgba(pat, t, 0, 0, 0, 0.3);
		else
			cairo_pattern_add_color_stop_rgba(pat, t, 0, 0, 1, 0.5);
		i++;
	}

	cairo_rectangle(cr, 0, 0, WIDTH, HEIGHT);
	cairo_set_source(cr, pat);
	cairo_fill(cr);

	cairo_pattern_destroy(pat);
}

void
circular_gradient_1(cairo_t *cr)
{
	cairo_pattern_t *r1;
	int i;

	cairo_set_source_rgba(cr, 0, 0, 0, 1);
	cairo_set_line_width(cr, 12);
	cairo_translate(cr, 0, 60);

	r1 = cairo_pattern_create_radial(30, 30, 10, 30, 30, 90);
	cairo_pattern_add_color_stop_rgba(r1, 0, 1, 1, 1, 1);
	cairo_pattern_add_color_stop_rgba(r1, 1, 0.6, 0.6, 0.6, 1);

	for (i = 0; i < 10; i++) {
		cairo_translate(cr, 90, 0);

		// as translation occurs, need to reset the source to get the same pattern at that location
		cairo_set_source(cr, r1);

		cairo_arc(cr, 0, 0, 40, 0, M_PI * 2);
		cairo_fill(cr);
	}

	cairo_pattern_destroy(r1);
}

void
circular_gradient_2(cairo_t *cr)
{
	cairo_pattern_t *r2;
	int i;

	cairo_translate(cr, 120, 60);
	r2 = cairo_pattern_create_radial(0, 0, 10, 0, 0, 40);
	cairo_pattern_add_color_stop_rgb(r2, 0, 1, 1, 0);
	cairo_pattern_add_color_stop_rgb(r2, 0.8, 0, 0, 0);

	for (i = 0; i < 10; i++) {

		cairo_set_source(cr, r2);
		cairo_arc(cr, 0, 0, 40, 0, M_PI * 2);
		cairo_fill(cr);

		cairo_translate(cr, 90, 90);
	}
	cairo_pattern_destroy(r2);
}

void
gen(const char *name, void (*render)(cairo_t *))
{
	cairo_surface_t *surface;
	cairo_t *cr;

	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, WIDTH, HEIGHT);
	cr = cairo_create(surface);

	render(cr);
	cairo_surface_write_to_png(surface, name);

	cairo_destroy(cr);
	cairo_surface_destroy(surface);
}

int
main(void)
{
	gen("linear_gradient_1.png", linear_gradient_1);
	gen("linear_gradient_2.png", linear_gradient_2);

	gen("circular_gradient_1.png", circular_gradient_1);
	gen("circular_gradient_2.png", circular_gradient_2);

	return 0;
}
