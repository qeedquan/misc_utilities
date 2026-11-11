#define STB_HBWANG_MAX_X 8192
#define STB_HBWANG_MAX_Y 8192
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_HERRINGBONE_WANG_TILE_IMPLEMENTATION
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include "stb_image.h"
#include "stb_image_write.h"
#include "stb_herringbone_wang_tile.h"

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
usage(void)
{
	fprintf(stderr, "usage: hwmapgen tile-file xsize ysize output.png\n");
	exit(2);
}

int
main(int argc, char *argv[])
{
	unsigned char *data;
	int xs, ys, w, h, rv;
	stbhw_tileset ts;

	if (argc != 5)
		usage();

	data = stbi_load(argv[1], &w, &h, NULL, 3);
	if (data == NULL)
		fatal("failed to load image file");

	xs = atoi(argv[2]);
	ys = atoi(argv[3]);

	if (xs < 1 || xs > STB_HBWANG_MAX_X)
		fatal("xsize invalid or out of range");

	if (ys < 1 || ys > STB_HBWANG_MAX_Y)
		fatal("ysize invalid or out of range");

	rv = stbhw_build_tileset_from_image(&ts, data, w * 3, w, h);
	if (rv == 0)
		fatal("failed to build tileset: %s", stbhw_get_last_error());

	free(data);

	data = malloc(3 * xs * ys);
	if (data == NULL)
		fatal("malloc: %s", strerror(errno));

	srand(time(NULL));
	rv = stbhw_generate_image(&ts, NULL, data, xs * 3, xs, ys);
	if (rv == 0)
		fatal("failed to generate tileset image: %s", stbhw_get_last_error());

	rv = stbi_write_png(argv[4], xs, ys, 3, data, xs * 3);
	if (rv == 0)
		fatal("failed to write to png file");

	stbhw_free_tileset(&ts);
	free(data);

	return 0;
}
