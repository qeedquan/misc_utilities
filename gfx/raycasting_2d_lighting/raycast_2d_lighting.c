// ported from https://ncase.me/sight-and-light/

#define _GNU_SOURCE
#include <stdio.h>
#include <math.h>
#include <SDL.h>
#include <cairo.h>
#include <err.h>

#define nelem(x) (sizeof(x) / sizeof(x[0]))

typedef struct {
	double x, y, r, t;
} vec_t;

vec_t segments[][2] = {
    // Border
    {{.x = 0, .y = 0}, {.x = 840, .y = 0}},
    {{.x = 840, .y = 0}, {.x = 840, .y = 360}},
    {{.x = 840, .y = 360}, {.x = 0, .y = 360}},
    {{.x = 0, .y = 360}, {.x = 0, .y = 0}},

    // Polygon #1
    {{.x = 100, .y = 150}, {.x = 120, .y = 50}},
    {{.x = 120, .y = 50}, {.x = 200, .y = 80}},
    {{.x = 200, .y = 80}, {.x = 140, .y = 210}},
    {{.x = 140, .y = 210}, {.x = 100, .y = 150}},

    // Polygon #2
    {{.x = 100, .y = 200}, {.x = 120, .y = 250}},
    {{.x = 120, .y = 250}, {.x = 60, .y = 300}},
    {{.x = 60, .y = 300}, {.x = 100, .y = 200}},

    // Polygon #3
    {{.x = 200, .y = 260}, {.x = 220, .y = 150}},
    {{.x = 220, .y = 150}, {.x = 300, .y = 200}},
    {{.x = 300, .y = 200}, {.x = 350, .y = 320}},
    {{.x = 350, .y = 320}, {.x = 200, .y = 260}},

    // Polygon #4
    {{.x = 540, .y = 60}, {.x = 560, .y = 40}},
    {{.x = 560, .y = 40}, {.x = 570, .y = 70}},
    {{.x = 570, .y = 70}, {.x = 540, .y = 60}},

    // Polygon #5
    {{.x = 650, .y = 190}, {.x = 760, .y = 170}},
    {{.x = 760, .y = 170}, {.x = 740, .y = 270}},
    {{.x = 740, .y = 270}, {.x = 630, .y = 290}},
    {{.x = 630, .y = 290}, {.x = 650, .y = 190}},

    // Polygon #6
    {{.x = 600, .y = 95}, {.x = 780, .y = 50}},
    {{.x = 780, .y = 50}, {.x = 680, .y = 150}},
    {{.x = 680, .y = 150}, {.x = 600, .y = 95}},
};

SDL_Window *window;
SDL_Renderer *renderer;

SDL_Texture *texture;

cairo_t *cr;
cairo_t *cl;
cairo_surface_t *canvas;
cairo_surface_t *lightmap;
cairo_surface_t *foreground;
cairo_surface_t *background;

void
event(void)
{
	SDL_Event ev;

	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
		case SDL_QUIT:
			exit(0);
		case SDL_KEYDOWN:
			switch (ev.key.keysym.sym) {
			case SDLK_ESCAPE:
				exit(0);
			}
		}
	}
}

vec_t
get_intersection(vec_t ray[], vec_t segment[])
{
	double r_px, r_py;
	double r_dx, r_dy;
	double s_px, s_py;
	double s_dx, s_dy;
	double r_mag, s_mag;
	double T1, T2;
	vec_t p;

	p = (vec_t){.t = -1};

	// RAY in parametric: Point + Delta*T1
	r_px = ray[0].x;
	r_py = ray[0].y;
	r_dx = ray[1].x - ray[0].x;
	r_dy = ray[1].y - ray[0].y;

	// SEGMENT in parametric: Point + Delta*T2
	s_px = segment[0].x;
	s_py = segment[0].y;
	s_dx = segment[1].x - segment[0].x;
	s_dy = segment[1].y - segment[0].y;

	// Are they parallel? If so, no intersect
	r_mag = sqrt(r_dx * r_dx + r_dy * r_dy);
	s_mag = sqrt(s_dx * s_dx + s_dy * s_dy);
	if (r_dx / r_mag == s_dx / s_mag && r_dy / r_mag == s_dy / s_mag) {
		// Unit vectors are the same.
		return p;
	}

	// SOLVE FOR T1 & T2
	// r_px+r_dx*T1 = s_px+s_dx*T2 && r_py+r_dy*T1 = s_py+s_dy*T2
	// ==> T1 = (s_px+s_dx*T2-r_px)/r_dx = (s_py+s_dy*T2-r_py)/r_dy
	// ==> s_px*r_dy + s_dx*T2*r_dy - r_px*r_dy = s_py*r_dx + s_dy*T2*r_dx - r_py*r_dx
	// ==> T2 = (r_dx*(s_py-r_py) + r_dy*(r_px-s_px))/(s_dx*r_dy - s_dy*r_dx)
	T2 = (r_dx * (s_py - r_py) + r_dy * (r_px - s_px)) / (s_dx * r_dy - s_dy * r_dx);
	T1 = (s_px + s_dx * T2 - r_px) / r_dx;

	// Must be within parametic whatevers for RAY/SEGMENT
	if (T1 < 0)
		return p;

	if (T2 < 0 || T2 > 1)
		return p;

	p.x = r_px + (r_dx * T1);
	p.y = r_py + (r_dy * T1);
	p.t = T1;
	return p;
}

int
compare_angle(const void *a, const void *b)
{
	const vec_t *p, *q;

	p = a;
	q = b;
	if (p->r < q->r)
		return -1;
	if (p->r > q->r)
		return 1;
	return 0;
}

int
has_point(vec_t *points, size_t npoints, vec_t p)
{
	size_t i;

	for (i = 0; i < npoints; i++) {
		if (points[i].x == p.x && points[i].y == p.y)
			return 1;
	}
	return 0;
}

size_t
get_sight_polygon(double sightx, double sighty, vec_t intersects[])
{
	vec_t points[64];
	vec_t closest_intersect;
	vec_t intersect;
	vec_t ray[2];

	double angles[64];
	double theta;

	size_t npoints;
	size_t nangles;
	size_t nintersects;
	size_t i, j;

	npoints = 0;
	for (i = 0; i < nelem(segments); i++) {
		for (j = 0; j < 2; j++) {
			if (!has_point(points, npoints, segments[i][j]))
				points[npoints++] = segments[i][j];
		}
	}

	nangles = 0;
	for (i = 0; i < npoints; i++) {
		theta = atan2(points[i].y - sighty, points[i].x - sightx);
		angles[nangles++] = theta - 0.00001;
		angles[nangles++] = theta + 0.00001;
	}

	nintersects = 0;
	for (i = 0; i < nangles; i++) {
		theta = angles[i];
		ray[0] = (vec_t){.x = sightx, .y = sighty};
		ray[1] = (vec_t){.x = sightx + cos(theta), .y = sighty + sin(theta)};

		closest_intersect.t = -1;
		for (j = 0; j < nelem(segments); j++) {
			intersect = get_intersection(ray, segments[j]);
			if (intersect.t < 0)
				continue;

			if (closest_intersect.t < 0 || intersect.t < closest_intersect.t)
				closest_intersect = intersect;
		}

		if (closest_intersect.t < 0)
			continue;
		closest_intersect.r = theta;

		intersects[nintersects++] = closest_intersect;
	}
	qsort(intersects, nintersects, sizeof(*intersects), compare_angle);

	return nintersects;
}

void
draw_polygon(vec_t *polygons, size_t npolygons, double r, double g, double b, double a)
{
	size_t i;

	cairo_set_source_rgba(cl, r, g, b, a);
	cairo_set_line_width(cl, 1);
	for (i = 0; i < npolygons; i++) {
		cairo_line_to(cl, polygons[i].x, polygons[i].y);
	}
	cairo_fill(cl);
}

void
draw_scene(void)
{
	vec_t polygons[256];
	size_t npolygons;

	double fuzzy_radius;
	double theta;
	double dx, dy;

	int mousex, mousey;

	SDL_GetMouseState(&mousex, &mousey);
	fuzzy_radius = 10;

	// Draw background
	cairo_set_source_surface(cr, background, 0, 0);
	cairo_paint(cr);

	// Draw polygons on the lightmap to render
	cairo_set_source_rgba(cl, 0, 0, 0, 0);
	cairo_set_operator(cl, CAIRO_OPERATOR_CLEAR);
	cairo_paint_with_alpha(cl, 1);
	cairo_set_operator(cl, CAIRO_OPERATOR_OVER);

	npolygons = get_sight_polygon(20, mousey, polygons);
	draw_polygon(polygons, npolygons, 1, 1, 1, 1);

	npolygons = get_sight_polygon(820, 360 - mousey, polygons);
	draw_polygon(polygons, npolygons, 1, 1, 1, 1);

	for (theta = 0; theta < 2 * M_PI; theta += (2 * M_PI) / 10) {
		dx = cos(theta) * fuzzy_radius;
		dy = sin(theta) * fuzzy_radius;

		npolygons = get_sight_polygon(20 + dx, mousey + dy, polygons);
		draw_polygon(polygons, npolygons, 1, 1, 1, 0.2);

		npolygons = get_sight_polygon(820 + dx, mousey + dy, polygons);
		draw_polygon(polygons, npolygons, 1, 1, 1, 0.2);
	}

	// Blend foreground with background using the lightmap generated
	cairo_set_source_surface(cr, foreground, 0, 0);
	cairo_mask_surface(cr, lightmap, 0, 0);
	cairo_fill(cr);

	// Draw dots
	cairo_set_source_rgb(cr, 0xff, 0xff, 0xff);
	cairo_arc(cr, 20, mousey, 4, 0, 2 * M_PI);
	cairo_fill(cr);
	cairo_arc(cr, 820, 360 - mousey, 4, 0, 2 * M_PI);
	cairo_fill(cr);
	for (theta = 0; theta < 2 * M_PI; theta += (2 * M_PI) / 10) {
		dx = cos(theta) * fuzzy_radius;
		dy = sin(theta) * fuzzy_radius;

		cairo_arc(cr, 20 + dx, mousey + dy, 4, 0, 2 * M_PI);
		cairo_fill(cr);
		cairo_arc(cr, 820 + dx, 360 - mousey + dy, 4, 0, 2 * M_PI);
		cairo_fill(cr);
	}
}

void
draw(void)
{
	void *data, *pixels;
	int pitch, height;

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	draw_scene();

	SDL_LockTexture(texture, NULL, &pixels, &pitch);
	data = cairo_image_surface_get_data(canvas);
	height = cairo_image_surface_get_height(canvas);
	memcpy(pixels, data, pitch * height);
	SDL_UnlockTexture(texture);

	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

int
main(void)
{
	int width, height;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
		errx(1, "Failed to init SDL: %s", SDL_GetError());

	width = 840;
	height = 360;
	if (SDL_CreateWindowAndRenderer(width, height, SDL_WINDOW_RESIZABLE, &window, &renderer) < 0)
		errx(1, "Failed to create window: %s", SDL_GetError());

	SDL_SetWindowTitle(window, "Raycast 2D Lighting");
	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	if (!texture)
		errx(1, "Failed to create texture: %s", SDL_GetError());

	canvas = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	cr = cairo_create(canvas);

	lightmap = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
	cl = cairo_create(lightmap);

	foreground = cairo_image_surface_create_from_png("foreground.png");
	background = cairo_image_surface_create_from_png("background.png");
	if (!foreground || !background)
		errx(1, "Failed to load image");

	for (;;) {
		event();
		draw();
	}

	return 0;
}
