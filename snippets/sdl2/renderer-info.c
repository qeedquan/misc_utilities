#include <assert.h>
#include <SDL.h>

int
main(void)
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_RendererInfo info;
	Uint32 i;
	int r;

	r = SDL_Init(SDL_INIT_VIDEO);
	assert(r >= 0);

	r = SDL_CreateWindowAndRenderer(1024, 768, SDL_WINDOW_RESIZABLE, &window, &renderer);
	assert(r >= 0);

	r = SDL_GetRendererInfo(renderer, &info);
	assert(r >= 0);

	printf("Name: %s\n", info.name);
	printf("Flags: %u\n", info.flags);
	printf("Number of Texture Formats: %u\n", info.num_texture_formats);
	printf("Formats:\n");
	for (i = 0; i < info.num_texture_formats; i++) {
		printf("\t%s\n", SDL_GetPixelFormatName(info.texture_formats[i]));
	}
	printf("Max Texture Size: %ux%u\n", info.max_texture_width, info.max_texture_height);
	printf("Window Pixel Format: %s\n", SDL_GetPixelFormatName(SDL_GetWindowPixelFormat(window)));

	SDL_Quit();
	return 0;
}
