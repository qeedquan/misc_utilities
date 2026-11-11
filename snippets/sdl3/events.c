// Print generated events to console
#include <stdio.h>
#include <SDL3/SDL.h>

int
main()
{
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event ev;

	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_Init fail : %s\n", SDL_GetError());
		return 1;
	}

	if (!SDL_CreateWindowAndRenderer("Event Handler", 640, 480, 0, &window, &renderer)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window: %s\n", SDL_GetError());
		return 1;
	}

	for (;;) {
		while (SDL_PollEvent(&ev)) {
			printf("Event Type: %#x\n", ev.type);
			switch (ev.type) {
			case SDL_EVENT_QUIT:
				return 0;

			case SDL_EVENT_KEY_DOWN:
			case SDL_EVENT_KEY_UP:
				printf("Key: %#x Scancode: %#x\n", ev.key.key, ev.key.scancode);
				printf("Mod: %#x Down: %d\n", ev.key.mod, ev.key.down);
				break;

			case SDL_EVENT_MOUSE_MOTION:
				printf("X: %f Y: %f\n", ev.motion.x, ev.motion.y);
				printf("RelX: %f RelY: %f\n", ev.motion.xrel, ev.motion.yrel);
				break;

			case SDL_EVENT_MOUSE_BUTTON_DOWN:
				printf("Down: %d X: %f Y: %f\n", ev.button.down, ev.button.x, ev.button.y);
				break;
			}
			printf("\n");
		}

		SDL_RenderPresent(renderer);
	}

	SDL_Quit();
	return 0;
}
