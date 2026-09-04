#include <assert.h>
#include <stdio.h>
#include <SDL.h>
#include "linalg.h"
#include "sdf.h"

enum {
	WIN = 4096,

	STROKE = 0,
	FILL = 1,
};

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;
SDL_Color style[32];
int strokeweight;
int flags;
uint8_t pixels[WIN * WIN * 4];

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

void
pixel(int x, int y, int r, int g, int b, int a)
{
	uint8_t *p;

	p = pixels + (y * WIN + x) * 4;
	p[0] = b;
	p[1] = g;
	p[2] = r;
	p[3] = a;
}

void
background(int r, int g, int b, int a)
{
	int x, y;

	for (y = 0; y < WIN; y++) {
		for (x = 0; x < WIN; x++) {
			pixel(x, y, r, g, b, a);
		}
	}
}

void
stroke(int r, int g, int b, int a)
{
	style[STROKE] = (SDL_Color){r, g, b, a};
}

void
fill(int r, int g, int b, int a)
{
	style[FILL] = (SDL_Color){r, g, b, a};
}

void
point(int x, int y)
{
	SDL_Color *col;

	x &= (WIN - 1);
	y &= (WIN - 1);
	col = style + STROKE;
	pixel(x, y, col->r, col->g, col->b, col->a);
}

void
circle(int xc, int yc, int r)
{
	int x0, y0;
	int x1, y1;
	int x, y;
	SDL_Color *col;

	x0 = xc - r;
	y0 = yc - r;
	x1 = xc + r;
	y1 = yc + r;
	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++) {
			float2 p = {{x - xc, y - yc}};
			float d = sdcircle(p, r);
			if (d <= 0) {
				col = style + FILL;
				pixel(x, y, col->r, col->g, col->b, col->a);
			}
		}
	}
}

void
rect(int x0, int y0, int w, int h)
{
	int x1, y1;
	int xc, yc;
	int x, y;
	SDL_Color *col;

	x1 = x0 + w;
	y1 = y0 + h;
	xc = (x0 + x1) / 2;
	yc = (y0 + y1) / 2;

	for (y = y0; y <= y1; y++) {
		for (x = x0; x <= x1; x++) {
			float2 p = {{x - xc, y - yc}};
			float2 b = {{w, h}};
			float d = sdrect(p, b);
			if (d <= 0) {
				col = style + FILL;
				pixel(x, y, col->r, col->g, col->b, col->a);
			}
		}
	}
}

void
triangle(int x0, int y0, int x1, int y1, int x2, int y2)
{
	int u0, v0;
	int u1, v1;
	int uc, vc;
	int u, v;
	SDL_Color *col;

	u0 = min(min(x0, x1), x2);
	u1 = max(max(x0, x1), x2);
	v0 = min(min(y0, y1), y2);
	v1 = max(max(y0, y1), y2);
	uc = (u0 + u1) / 2;
	vc = (v0 + v1) / 2;

	for (u = u0; u <= u1; u++) {
		for (v = v0; v <= v1; v++) {
			float2 p = {{u - uc, v - vc}};
			float2 p0 = {{x0 - uc, y0 - vc}};
			float2 p1 = {{x1 - uc, y1 - vc}};
			float2 p2 = {{x2 - uc, y2 - vc}};
			float d = sdtriangle(p, p0, p1, p2);
			if (d <= 0) {
				col = style + FILL;
				pixel(u, v, col->r, col->g, col->b, col->a);
			}
		}
	}
}

void
upload(void)
{
	int w, h, y, rv;
	SDL_Rect r;
	void *data;
	int pitch;
	uint8_t *p;

	SDL_GetWindowSize(window, &w, &h);
	assert(w <= WIN && h <= WIN);

	rv = SDL_LockTexture(texture, NULL, &data, &pitch);
	assert(rv >= 0);

	p = data;
	for (y = 0; y < h; y++) {
		memcpy(p + pitch * y, pixels + y * WIN * 4, 4 * w);
	}
	SDL_UnlockTexture(texture);

	r = (SDL_Rect){0, 0, w, h};
	SDL_RenderCopy(renderer, texture, &r, &r);
}

void
draw(void)
{
	background(150, 150, 150, 200);

	strokeweight = 4;
	stroke(0, 0, 0, 255);
	fill(255, 0, 0, 255);
	circle(512, 512, 50);
	fill(0, 255, 255, 255);
	circle(100, 100, 30);

	fill(50, 10, 80, 255);
	circle(200, 200, 50);

	rect(400, 400, 50, 80);

	fill(25, 80, 100, 255);
	triangle(600, 600, 680, 600, 700, 750);

	upload();
	SDL_RenderPresent(renderer);
}

int
main(void)
{
	int rv;

	rv = SDL_Init(SDL_INIT_VIDEO);
	assert(rv >= 0);

	rv = SDL_CreateWindowAndRenderer(1200, 800, SDL_WINDOW_RESIZABLE, &window, &renderer);
	assert(rv >= 0);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIN, WIN);
	assert(texture != NULL);

	for (;;) {
		event();
		draw();
	}
	return 0;
}