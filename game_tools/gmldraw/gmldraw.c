/*

Based on https://github.com/cfiraguston/GML_Draw

http://www.antonis.de/qbebooks/gwbasman/draw.html
Parses BASIC GML files and render the output to a file

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <SDL3/SDL.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

typedef struct {
	int width;
	int height;
} Flags;

typedef struct {
	SDL_Renderer *renderer;
	int x0, y0, x1, y1; // The boundary of the rendered drawing
	int x;              // Current position X
	int y;              // Current position Y
	int color;          // Current Color
	int scale;          // Current scale factor
	float angle;        // Current angle
	bool penUp;         // Current only move without draw
	bool returnOrigin;  // Current draw but return to origin point
} stDrawingContext;

typedef struct {
	Uint8 R;
	Uint8 G;
	Uint8 B;
} stRGB;

Flags flags = {
	.width = 320,
	.height = 200,
};

stRGB Pallete0[] = {
	{ 0x00, 0x00, 0x00 },
	{ 0x55, 0xFF, 0x55 },
	{ 0xFF, 0x55, 0x55 },
	{ 0xFF, 0xFF, 0x55 }
};

stRGB Pallete1[] = {
	{ 0x00, 0x00, 0x00 },
	{ 0x55, 0xFF, 0xFF },
	{ 0xFF, 0x55, 0xFF },
	{ 0xFF, 0xFF, 0xFF }
};

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
initGml(SDL_Surface **surface, SDL_Renderer **renderer, int width, int height, stDrawingContext *ctx)
{
	if (!SDL_Init(SDL_INIT_VIDEO))
		fatal("Failed to init SDL: %s", SDL_GetError());

	*surface = SDL_CreateSurface(width, height, SDL_PIXELFORMAT_ARGB8888);
	if (!*surface)
		fatal("Failed to create surface: %s", SDL_GetError());

	*renderer = SDL_CreateSoftwareRenderer(*surface);
	if (!*renderer)
		fatal("Failed to create renderer: %s", SDL_GetError());

	/* Initialize drawing context */
	ctx->renderer = *renderer;
	ctx->x = width / 2;
	ctx->y = height / 2;
	ctx->color = 1;
	ctx->scale = 4;
	ctx->angle = 0.0;
	ctx->penUp = false;
	ctx->returnOrigin = false;
	ctx->x0 = ctx->x;
	ctx->y0 = ctx->y;
	ctx->x1 = ctx->x;
	ctx->y1 = ctx->y;
	SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255);
	SDL_RenderClear(*renderer);
}

int
destroyGml(SDL_Surface **surface, SDL_Renderer **renderer)
{
	SDL_DestroySurface(*surface);
	SDL_DestroyRenderer(*renderer);
	SDL_Quit();
	return 0;
}

void
executeGmlCommandMove(stDrawingContext *ctx, int targetX, int targetY)
{
	int x0, y0;
	int x1, y1;

	if (!ctx->penUp) {
		SDL_RenderLine(ctx->renderer, ctx->x, ctx->y, targetX, targetY);

		x0 = SDL_min(ctx->x, targetX);
		y0 = SDL_min(ctx->y, targetY);
		x1 = SDL_max(ctx->x, targetX);
		y1 = SDL_max(ctx->y, targetY);

		ctx->x0 = SDL_min(ctx->x0, x0);
		ctx->y0 = SDL_min(ctx->y0, y0);
		ctx->x1 = SDL_max(ctx->x1, x1);
		ctx->y1 = SDL_max(ctx->y1, y1);

	} else {
		ctx->penUp = false;
	}

	if (!ctx->returnOrigin) {
		ctx->x = targetX;
		ctx->y = targetY;
	} else {
		ctx->returnOrigin = false;
	}
}

void
executeGmlCommand(stDrawingContext *ctx, char cmd, int param1, int param2)
{
	/* Execute command */
	switch (cmd) {
	case 'U': /* Move up		*/
		executeGmlCommandMove(ctx, ctx->x, ctx->y - ctx->scale * param1);
		break;
	case 'D': /* Move down	*/
		executeGmlCommandMove(ctx, ctx->x, ctx->y + ctx->scale * param1);
		break;
	case 'L': /* Move left	*/
		executeGmlCommandMove(ctx, ctx->x - ctx->scale * param1, ctx->y);
		break;
	case 'R': /* Move right	*/
		executeGmlCommandMove(ctx, ctx->x + ctx->scale * param1, ctx->y);
		break;
	case 'E': /* Move diagonally up and right		*/
		executeGmlCommandMove(ctx, ctx->x + ctx->scale * param1, ctx->y - ctx->scale * param1);
		break;
	case 'F': /* Move diagonally down and right	*/
		executeGmlCommandMove(ctx, ctx->x + ctx->scale * param1, ctx->y + ctx->scale * param1);
		break;
	case 'G': /* Move diagonally down and left	*/
		executeGmlCommandMove(ctx, ctx->x - ctx->scale * param1, ctx->y + ctx->scale * param1);
		break;
	case 'H': /* Move diagonally up and left		*/
		executeGmlCommandMove(ctx, ctx->x - ctx->scale * param1, ctx->y - ctx->scale * param1);
		break;
	case 'B': /* Move but plot no points							*/
		ctx->penUp = true;
		break;
	case 'N': /* Move but return to original position when done	*/
		ctx->returnOrigin = true;
		break;
	case 'C': /* Set color	*/
		ctx->color = param1;
		// TODO COLOR
		SDL_SetRenderDrawColor(ctx->renderer, Pallete1[ctx->color].R, Pallete1[ctx->color].G, Pallete1[ctx->color].B, 255);
		break;
	case 'S': /* Set scale	*/
		ctx->scale = param1 / 4;
		break;
	case 'M':
		executeGmlCommandMove(ctx, ctx->scale * param1, ctx->scale * param2);
		break;
	case 'A': /* Set angle	*/
		ctx->angle = param1 * 90.0f;
		break;
	case 'T': /* Turn angle	*/
		ctx->angle += param1;
		break;
	default:
		printf("Unkown command %c\n", cmd);
		break;
	}
}

void
executeGml(stDrawingContext *ctx, const char *commands)
{
	size_t len = strlen(commands);
	for (size_t i = 0; i < len; i++) {
		char cmd = commands[i];
		int n = 0;

		if (isalpha(cmd) != 0) {
			cmd = toupper(cmd);
		}

		if ((cmd == ';') || (cmd == ' ') || (cmd == ',')) {
			/* Do nothing */
			/* Ignore spaces */
			/* Ignore commas */
		} else if ((cmd == 'B') || (cmd == 'N')) {
			executeGmlCommand(ctx, cmd, -1, -1);
		} else if (cmd == 'M') {
			int x = 0;
			int y = 0;
			int nTotal = 0;
			char sign = ' ';

			if (('+' == commands[i + 1]) || ('-' == commands[i + 1])) {
				sign = commands[i + 1];
				i++;
			}
			/* Last parameter of sscanf %n holds the amount of characters matched */
			if (sscanf(&(commands[i + 1]), "%d,%d %n", &x, &y, &nTotal) == 2) {
				/* Check if a there was a sign before the x number */
				if (('+' == sign) || ('-' == sign)) {
					/* If required convert it to negative */
					if ('-' == sign) {
						x *= -1;
					}

					/* Divide by scale since context x and y are already scaled after drawing */
					x += (ctx->x / ctx->scale);
					y += (ctx->y / ctx->scale);
				}
				i += nTotal;
				executeGmlCommand(ctx, cmd, x, y);
			}
		} else if (isdigit(commands[i + 1]) != 0) { /* Parse numeric argument if present */
			char *tempPtr = (char *)&commands[i + 1];
			n = strtol(&(commands[i + 1]), (char **)&tempPtr, 10);
			executeGmlCommand(ctx, cmd, n, -1);
			i += tempPtr - &(commands[i + 1]);
		} else {
			printf("Unexpected GML character %c\n", commands[i + 1]);
			break;
		}
	}
}

void
usage()
{
	fprintf(stderr, "usage: [options] <input> <output>\n");
	fprintf(stderr, "  -w: specify width (default: %d)\n", flags.width);
	fprintf(stderr, "  -h: specify height (default: %d)\n", flags.height);
	fprintf(stderr, "\n");
	exit(2);
}

void
parseflags(int *argc, char ***argv)
{
	int c;

	while ((c = getopt(*argc, *argv, "w:h:")) != -1) {
		switch (c) {
		case 'w':
			flags.width = atoi(optarg);
			break;
		case 'h':
			flags.height = atoi(optarg);
			break;
		}
	}
	*argc -= optind;
	*argv += optind;

	if (*argc < 2)
		usage();
}

void
parsegml(stDrawingContext *Context, const char *name)
{
	FILE *fp;
	char line[1024];
	int len;

	fp = fopen(name, "rb");
	if (!fp)
		fatal("Failed to open file %s: %s", strerror(errno));

	while (fgets(line, sizeof(line), fp)) {
		len = strlen(line);
		if (len > 0)
			line[--len] = '\0';
		executeGml(Context, line);
	}
}

int
main(int argc, char *argv[])
{
	SDL_Surface *surface;
	SDL_Renderer *renderer = NULL;
	stDrawingContext Context;

	parseflags(&argc, &argv);
	initGml(&surface, &renderer, flags.width, flags.height, &Context);

	parsegml(&Context, argv[0]);

	SDL_RenderPresent(renderer);

	printf("Bounds: (%d, %d) - (%d, %d)\n", Context.x0, Context.y0, Context.x1, Context.y1);
	printf("Writing image to file: %s\n", argv[1]);
	if (!stbi_write_png(argv[1], surface->w, surface->h, 4, surface->pixels, surface->pitch))
		fatal("Failed to write image");

	return destroyGml(&surface, &renderer);
}
