#include <cairo.h>

int
main(void)
{
	cairo_t *ctx;
	cairo_surface_t *surface;
	cairo_pattern_t *pattern;

	surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, 400, 300);

	ctx = cairo_create(surface);

	pattern = cairo_pattern_create_mesh();

	cairo_mesh_pattern_begin_patch(pattern);
	cairo_mesh_pattern_line_to(pattern, 0, 0);     /* Corner 0 */
	cairo_mesh_pattern_line_to(pattern, 400, 0);   /* Corner 1 */
	cairo_mesh_pattern_line_to(pattern, 400, 300); /* Corner 2 */
	cairo_mesh_pattern_line_to(pattern, 0, 300);   /* Corner 3 */
	cairo_mesh_pattern_line_to(pattern, 0, 0);     /* Back to corner 0 */

	cairo_mesh_pattern_set_corner_color_rgb(pattern, 0, 0, 0, 0);   /* Corner 0 - black */
	cairo_mesh_pattern_set_corner_color_rgb(pattern, 1, 0.5, 0, 0); /* Corner 1 - red */
	cairo_mesh_pattern_set_corner_color_rgb(pattern, 2, 0, 0.6, 0); /* Corner 2 - green */
	cairo_mesh_pattern_set_corner_color_rgb(pattern, 3, 0, 0, 0.7); /* Corner 2 - blue */

	cairo_mesh_pattern_end_patch(pattern);

	cairo_set_source(ctx, pattern);

	cairo_rectangle(ctx, 0, 0, 400, 300);
	cairo_fill(ctx);

	cairo_surface_write_to_png(surface, "mesh.png");

	return 0;
}
