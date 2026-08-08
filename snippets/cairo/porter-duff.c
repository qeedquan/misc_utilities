#include <string.h>
#include <cairo.h>
#include <cairo-svg.h>

void render(const char *name);

int
main(void)
{
	render("clear");

	render("source");
	render("over");
	render("in");
	render("out");
	render("atop");

	render("dest");
	render("dest_over");
	render("dest_in");
	render("dest_out");
	render("dest_atop");

	render("xor");
	render("add");
	render("saturate");

	render("multiply");
	render("screen");
	render("overlay");
	render("darken");
	render("lighten");
	render("color_dodge");
	render("color_burn");
	render("hard_light");
	render("soft_light");
	render("difference");
	render("exclusion");
	render("hsl_hue");
	render("hsl_saturation");
	render("hsl_color");
	render("hsl_luminosity");

	return 0;
}

void
render(const char *name)
{
	cairo_surface_t *surface;
	cairo_t *cr;

	char svg_filename[50];
	char png_filename[50];
	strcpy(svg_filename, name);
	strcpy(png_filename, name);
	strcat(svg_filename, ".svg");
	strcat(png_filename, ".png");

	surface = cairo_svg_surface_create(svg_filename, 160, 120);
	cr = cairo_create(surface);

	cairo_rectangle(cr, 0, 0, 120, 90);
	cairo_set_source_rgba(cr, 0.7, 0, 0, 0.8);
	cairo_fill(cr);

	if (strcmp(name, "clear") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);

	else if (strcmp(name, "source") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	else if (strcmp(name, "over") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
	else if (strcmp(name, "in") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_IN);
	else if (strcmp(name, "out") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_OUT);
	else if (strcmp(name, "atop") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_ATOP);

	else if (strcmp(name, "dest") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_DEST);
	else if (strcmp(name, "dest_over") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_DEST_OVER);
	else if (strcmp(name, "dest_in") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_DEST_IN);
	else if (strcmp(name, "dest_out") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_DEST_OUT);
	else if (strcmp(name, "dest_atop") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_DEST_ATOP);

	else if (strcmp(name, "xor") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_XOR);
	else if (strcmp(name, "add") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_ADD);
	else if (strcmp(name, "saturate") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_SATURATE);
	else if (strcmp(name, "multiply") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_MULTIPLY);
	else if (strcmp(name, "screen") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_SCREEN);
	else if (strcmp(name, "overlay") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_OVERLAY);
	else if (strcmp(name, "darken") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_DARKEN);
	else if (strcmp(name, "lighten") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_LIGHTEN);
	else if (strcmp(name, "color_dodge") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_COLOR_DODGE);
	else if (strcmp(name, "color_burn") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_COLOR_BURN);
	else if (strcmp(name, "hard_light") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_HARD_LIGHT);
	else if (strcmp(name, "soft_light") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_SOFT_LIGHT);
	else if (strcmp(name, "difference") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
	else if (strcmp(name, "exclusion") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_EXCLUSION);
	else if (strcmp(name, "hsl_hue") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_HSL_HUE);
	else if (strcmp(name, "hsl_saturation") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_HSL_SATURATION);
	else if (strcmp(name, "hsl_color") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_HSL_COLOR);
	else if (strcmp(name, "hsl_luminosity") == 0)
		cairo_set_operator(cr, CAIRO_OPERATOR_HSL_LUMINOSITY);

	cairo_rectangle(cr, 40, 30, 120, 90);
	cairo_set_source_rgba(cr, 0, 0, 0.9, 0.4);
	cairo_fill(cr);

	cairo_destroy(cr);
	cairo_surface_write_to_png(surface, png_filename);
	cairo_surface_destroy(surface);
}
