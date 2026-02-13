#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void
draw_chess_board(SDL_Renderer *renderer)
{
	int row = 0, column = 0, x = 0;
	SDL_FRect rect;
	SDL_Rect darea;

	SDL_GetRenderViewport(renderer, &darea);

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);

	for (; row < 8; row++) {
		column = row % 2;
		x = column;
		for (; column < 4 + (row % 2); column++) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);

			rect.w = (float)darea.w / 8;
			rect.h = (float)darea.h / 8;
			rect.x = x * rect.w;
			rect.y = row * rect.h;
			x = x + 2;
			SDL_RenderFillRect(renderer, &rect);
		}
	}
	SDL_RenderPresent(renderer);
}

int
main()
{
	SDL_Surface *surface;
	SDL_Renderer *renderer;

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init fail : %s\n", SDL_GetError());
		return 1;
	}

	surface = SDL_CreateSurface(640, 480, SDL_PIXELFORMAT_ARGB8888);
	renderer = SDL_CreateSoftwareRenderer(surface);
	if (!renderer) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Render creation for surface fail : %s\n", SDL_GetError());
		return 1;
	}

	draw_chess_board(renderer);
	stbi_write_png("chessboard.png", surface->w, surface->h, 4, surface->pixels, surface->pitch);

	SDL_Quit();
	return 0;
}
