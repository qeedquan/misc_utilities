/*

https://create.stephan-brumme.com/antialiased-circle/

*/

#include <stdio.h>
#include <math.h>
#include <SDL.h>

SDL_Window *window;
SDL_Renderer *renderer;

float ra, rb;
int flag;
int render_mode;
int blend_mode;

void
setpixel4(float xc, float yc, float dx, float dy, SDL_Color col)
{
	SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
	SDL_RenderDrawPointF(renderer, xc + dx, yc + dy);
	SDL_RenderDrawPointF(renderer, xc - dx, yc + dy);
	SDL_RenderDrawPointF(renderer, xc + dx, yc - dy);
	SDL_RenderDrawPointF(renderer, xc - dx, yc - dy);
}

void
wuellipse(float xc, float yc, float a, float b, SDL_Color col, int flag)
{
	static const int max_transparency = 255;

	SDL_Color alpha, alpha2;
	float transparency;
	float quarter;
	float error;
	float a2, b2;
	float x, y;

	a2 = a * a;
	b2 = b * b;
	quarter = round(a2 / sqrt(a2 + b2));
	for (x = 0; x <= quarter; x++) {
		y = b * sqrt(1 - x * x / a2);
		error = y - floor(y);
		transparency = round(error * max_transparency);

		alpha = alpha2 = col;
		alpha.a = transparency;
		alpha2.a = max_transparency - transparency;

		if (!(flag & 1))
			alpha.a = alpha2.a = 255;

		setpixel4(xc, yc, x, floor(y), alpha);
		setpixel4(xc, yc, x, floor(y) + 1, alpha2);
	}

	quarter = round(b2 / sqrt(a2 + b2));
	for (y = 0; y <= quarter; y++) {
		x = a * sqrt(1 - y * y / b2);
		error = x - floor(x);
		transparency = round(error * max_transparency);

		alpha = alpha2 = col;
		alpha.a = transparency;
		alpha2.a = max_transparency - transparency;

		if (!(flag & 1))
			alpha.a = alpha2.a = 255;

		setpixel4(xc, yc, floor(x), y, alpha);
		setpixel4(xc, yc, floor(x) + 1, y, alpha2);
	}
}

void
imellipse(float cx, float cy, float a, float b, SDL_Color col, int flag)
{
	static const float max_transparency = 255;

	SDL_Color alpha;
	float thickness;
	float one, error;
	float x, y;

	thickness = 2.0f / fmin(a, b);
	for (x = 0.0f; x <= a + 1; x++) {
		for (y = 0.0f; y <= b + 1; y++) {
			one = (x * x) / (a * a) + (y * y) / (b * b);
			error = (one - 1) / thickness;

			if (error > 1)
				break;
			if (error < -1)
				continue;

			alpha = col;
			alpha.a = round(fabs(error) * max_transparency);
			if (!(flag & 1))
				alpha.a = 255;

			setpixel4(cx, cy, x, y, alpha);
		}
	}
}

void
fatal(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fprintf(stderr, "\n");
	exit(1);
}

void
initsdl(void)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		fatal("Failed to init SDL: %s", SDL_GetError());

	if (SDL_CreateWindowAndRenderer(1024, 768, SDL_WINDOW_RESIZABLE, &window, &renderer) < 0)
		fatal("Failed to create window: %s", SDL_GetError());

	SDL_SetWindowTitle(window, "Ellipses");

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
}

void
reset(void)
{
	ra = 300;
	rb = 300;
	flag = 0;
	render_mode = 1;
	blend_mode = 1;
}

void
event(void)
{
	static const SDL_BlendMode blends[] = {
	    SDL_BLENDMODE_NONE,
	    SDL_BLENDMODE_BLEND,
	    SDL_BLENDMODE_ADD,
	    SDL_BLENDMODE_MOD,
	};

	static const float step = 5;
	SDL_Event ev;

	while (SDL_PollEvent(&ev)) {
		switch (ev.type) {
		case SDL_QUIT:
			exit(0);
		case SDL_KEYDOWN:
			switch (ev.key.keysym.sym) {
			case SDLK_ESCAPE:
				exit(0);
			case SDLK_LEFT:
				ra -= step;
				break;
			case SDLK_RIGHT:
				ra += step;
				break;
			case SDLK_UP:
				rb -= step;
				break;
			case SDLK_DOWN:
				rb += step;
				break;
			case SDLK_1:
				render_mode = 1;
				break;
			case SDLK_2:
				render_mode = 2;
				break;
			case SDLK_SPACE:
				flag ^= 1;
				break;
			case SDLK_a:
				if (blend_mode > 0)
					blend_mode--;
				break;
			case SDLK_s:
				if (blend_mode < 3)
					blend_mode++;
				break;
			}

			ra = fabs(ra);
			rb = fabs(rb);
			SDL_SetRenderDrawBlendMode(renderer, blends[blend_mode]);

			printf("render mode: %d blend mode: %d flag: %d ra: %f rb: %f\n", render_mode, blend_mode, flag, ra, rb);
			break;
		}
	}
}

void
draw(void)
{
	SDL_Color col = {255, 0, 0, 255};
	int xc, yc;

	xc = 512;
	yc = 384;

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);
	switch (render_mode) {
	case 1:
		imellipse(xc, yc, ra, rb, col, flag);
		break;
	case 2:
		wuellipse(xc, yc, ra, rb, col, flag);
		break;
	}
	SDL_RenderPresent(renderer);
}

int
main(void)
{
	initsdl();
	reset();
	for (;;) {
		event();
		draw();
	}

	return 0;
}
