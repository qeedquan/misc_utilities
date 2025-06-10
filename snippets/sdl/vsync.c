#include <assert.h>
#include <SDL.h>

int
main(int argc, char **)
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event ev;
	char text[128];
	Uint32 start, now;
	int width, height;
	int flag;
	int frame;
	int r;
	SDL_Rect rect;

	flag = 0;
	if (argc >= 2) {
		flag = SDL_RENDERER_PRESENTVSYNC;
		printf("Turning on vsync\n");
	}

	r = SDL_Init(SDL_INIT_VIDEO);
	assert(r >= 0);

	width = 800;
	height = 600;
	window = SDL_CreateWindow("Vertical Sync Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE);
	assert(window != NULL);

	renderer = SDL_CreateRenderer(window, -1, flag | SDL_RENDERER_ACCELERATED);
	assert(renderer != NULL);

	start = SDL_GetTicks();
	frame = 0;
	for (;;) {
		while (SDL_PollEvent(&ev)) {
			switch (ev.type) {
			case SDL_QUIT:
				return 0;
			}
		}

		now = SDL_GetTicks();
		if ((now - start) >= 1000) {
			sprintf(text, "Frame: %d", frame);
			SDL_SetWindowTitle(window, text);
			start = now;
			frame = 0;
		}

		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		rect.x = 0;
		rect.y = 0;
		rect.w = width;
		rect.h = height;
		SDL_RenderDrawRect(renderer, &rect);
		SDL_RenderPresent(renderer);
		frame++;
	}

	SDL_Quit();
	return 0;
}
