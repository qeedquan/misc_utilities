/*

A quad can be broken up into 2 triangles.
Following code shows how to set the triangle vertices/colors such that it draws like
one quad with a linear gradient in vertical/horizontal direction.

*/

#include <assert.h>
#include <SDL3/SDL.h>

SDL_Window *window;
SDL_Renderer *renderer;

void
render_linear_gradient_horizontal(SDL_FRect rect, SDL_FColor color1, SDL_FColor color2)
{
	SDL_Vertex vert[6];

	vert[0].position.x = rect.x;
	vert[0].position.y = rect.y;
	vert[0].color = color1;

	vert[1].position.x = rect.x + rect.w;
	vert[1].position.y = rect.y;
	vert[1].color = color2;

	vert[2].position.x = rect.x;
	vert[2].position.y = rect.y + rect.h;
	vert[2].color = color1;

	vert[3].position.x = rect.x;
	vert[3].position.y = rect.y + rect.h;
	vert[3].color = color1;

	vert[4].position.x = rect.x + rect.w;
	vert[4].position.y = rect.y + rect.h;
	vert[4].color = color2;

	vert[5].position.x = rect.x + rect.w;
	vert[5].position.y = rect.y;
	vert[5].color = color2;

	SDL_RenderGeometry(renderer, NULL, vert, 6, NULL, 0);
}

void
render_linear_gradient_vertical(SDL_FRect rect, SDL_FColor color1, SDL_FColor color2)
{
	SDL_Vertex vert[6];

	vert[0].position.x = rect.x;
	vert[0].position.y = rect.y;
	vert[0].color = color1;

	vert[1].position.x = rect.x + rect.w;
	vert[1].position.y = rect.y;
	vert[1].color = color1;

	vert[2].position.x = rect.x;
	vert[2].position.y = rect.y + rect.h;
	vert[2].color = color2;

	vert[3].position.x = rect.x + rect.w;
	vert[3].position.y = rect.y;
	vert[3].color = color1;

	vert[4].position.x = rect.x;
	vert[4].position.y = rect.y + rect.h;
	vert[4].color = color2;

	vert[5].position.x = rect.x + rect.w;
	vert[5].position.y = rect.y + rect.h;
	vert[5].color = color2;

	SDL_RenderGeometry(renderer, NULL, vert, 6, NULL, 0);
}

int
main()
{
	int width = 800;
	int height = 600;

	SDL_FColor color1 = { 0.007843, 0.000000, 0.141176, 1 };
	SDL_FColor color2 = { 0.000000, 0.831373, 1.000000, 1 };
	SDL_FRect rect1 = { 0, 0, width, height / 2 };
	SDL_FRect rect2 = { 0, height / 2, width, height / 2 };

	window = SDL_CreateWindow("Linear Gradient Quad from Triangle", width, height, 0);
	assert(window);

	renderer = SDL_CreateRenderer(window, NULL);
	assert(renderer);

	for (;;) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev) != 0) {
			switch (ev.type) {
			case SDL_EVENT_QUIT:
				return 0;
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		render_linear_gradient_horizontal(rect1, color1, color2);
		render_linear_gradient_vertical(rect2, color1, color2);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
