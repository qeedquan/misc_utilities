// Ported from https://gist.github.com/rexim/c1e3c1b99cb824ab6c817c3f45d75564

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <SDL3/SDL.h>

#define rad2deg(x) ((x) / SDL_PI_F * 180)

#define FPS 60

void *
xcalloc(size_t nmemb, size_t size)
{
	void *ptr;

	ptr = calloc(nmemb, size);
	if (!ptr)
		abort();
	return ptr;
}

float
distance(SDL_FPoint p1, SDL_FPoint p2)
{
	return hypot(p2.x - p1.x, p2.y - p1.y);
}

SDL_Texture *
make_circle_texture(SDL_Renderer *renderer, int width, int height, int radius)
{
	SDL_Surface *surface;
	SDL_Texture *texture;
	uint32_t *pixels;
	int x, y, dx, dy;
	int index;

	pixels = xcalloc(height * width, sizeof(*pixels));
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			dx = width / 2 - x;
			dy = height / 2 - y;
			if (dx * dx + dy * dy < radius * radius) {
				index = (y * width) + x;
				pixels[index] = 0xFFFFFFFF;
			}
		}
	}
	surface = SDL_CreateSurfaceFrom(width, height, SDL_PIXELFORMAT_RGBA8888, pixels, width * 4);
	assert(surface);

	texture = SDL_CreateTextureFromSurface(renderer, surface);
	assert(texture);
	return texture;
}

int
main()
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event ev;

	SDL_FRect src_rect;
	SDL_FRect dst_rect;

	SDL_FPoint center;
	SDL_FPoint ball;
	SDL_FPoint smear;
	SDL_FPoint target;
	float angle;
	float speed;
	float dt;

	SDL_Texture *circle_texture;
	int circle_radius;
	int circle_width;
	int circle_height;

	ball.x = 0;
	ball.y = 0;
	smear = ball;
	target.x = 400;
	target.y = 300;
	circle_radius = 200;
	circle_width = circle_radius * 2;
	circle_height = circle_radius * 2;

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to init SDL: %s\n", SDL_GetError());
		return 1;
	}

	if (!SDL_CreateWindowAndRenderer("Blob", 800, 600, 0, &window, &renderer)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window: %s\n", SDL_GetError());
		return 1;
	}

	circle_texture = make_circle_texture(renderer, circle_width, circle_height, circle_radius);
	for (;;) {
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_EVENT_QUIT:
				return 0;

			case SDL_EVENT_KEY_DOWN:
				switch (ev.key.key) {
				case SDLK_ESCAPE:
					return 0;
				}
				break;

			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				target.x = ev.button.x;
				target.y = ev.button.y;
				break;
			}
		}

		dt = 1.0 / FPS;
		speed = 15.0;
		ball.x += dt * (target.x - ball.x) * speed;
		ball.y += dt * (target.y - ball.y) * speed;
		smear.x += dt * (ball.x - smear.x) * speed * 0.45;
		smear.y += dt * (ball.y - smear.y) * speed * 0.45;

		SDL_SetRenderDrawColor(renderer, 0x18, 0x18, 0x18, 0xFF);
		SDL_RenderClear(renderer);

		src_rect = (SDL_FRect){
			0.0,
			0.0,
			circle_width,
			circle_height,
		};
		dst_rect = (SDL_FRect){
			ball.x,
			ball.y,
			circle_width * 0.25,
			circle_height * 0.25,
		};
		dst_rect.x -= dst_rect.w * 0.5;
		dst_rect.y -= dst_rect.h * 0.5;

		SDL_SetTextureColorMod(circle_texture, 0x00, 0xFF, 0x00);
		SDL_RenderTextureRotated(renderer, circle_texture, &src_rect, &dst_rect, 0.0, NULL, SDL_FLIP_NONE);

		src_rect = (SDL_FRect){
			0.0,
			0.0,
			circle_width,
			circle_height * 0.5,
		};
		dst_rect = (SDL_FRect){
			0.0,
			0.0,
			circle_width * 0.25,
			distance(ball, smear),
		};
		dst_rect.x = ball.x - dst_rect.w * 0.5;
		dst_rect.y = ball.y - dst_rect.h;
		SDL_SetTextureColorMod(circle_texture, 0x00, 0xFF, 0x00);
		center = (SDL_FPoint){
			dst_rect.w * 0.5,
			dst_rect.h,
		};
		angle = rad2deg(atan2(smear.y - ball.y, smear.x - ball.x)) + 90;
		SDL_RenderTextureRotated(renderer, circle_texture, &src_rect, &dst_rect, angle, &center, SDL_FLIP_NONE);

		SDL_RenderPresent(renderer);
		SDL_Delay(1000.0 / FPS);
	}

	SDL_Quit();
	return 0;
}
