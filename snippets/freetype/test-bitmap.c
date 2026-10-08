#include <stdint.h>
#include <err.h>
#include <errno.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define max(a, b) (((a) > (b)) ? (a) : (b))
#define min(a, b) (((a) < (b)) ? (a) : (b))

unsigned
npot(unsigned x)
{
	unsigned p;

	p = 1;
	while (p < x)
		p <<= 1;
	return p;
}

int
main(int argc, char *argv[])
{
	FT_Library ft;
	FT_Face face;
	FT_Glyph_Metrics *metrics;
	FT_Bitmap *bmp;
	FILE *fp;
	uint8_t *data, *hdr, *img, *pix, *bmppix;
	unsigned width, height, total_width, total_height, hdrsz;
	unsigned x, y;
	int ch, r;

	if (argc != 4) {
		fprintf(stderr, "usage: <font_file> <pixel_size> <out.tga>\n");
		exit(2);
	}
	fp = fopen(argv[3], "wb");
	if (!fp)
		errx(1, "Failed to open file: %s: %s", argv[3], strerror(errno));

	r = FT_Init_FreeType(&ft);
	if (r)
		errx(1, "Failed to init freetype: %s", FT_Error_String(r));

	r = FT_New_Face(ft, argv[1], 0, &face);
	if (r)
		errx(1, "Failed to load font: %s: %s", argv[1], FT_Error_String(r));
	FT_Set_Pixel_Sizes(face, 0, atoi(argv[2]));

	width = height = 0;
	for (ch = 32; ch <= 127; ch++) {
		FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT);
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
		bmp = &face->glyph->bitmap;
		metrics = &face->glyph->metrics;

		width = max(width, bmp->width);
		height = max(height, bmp->rows);

		printf("%c (%d) dimension (%ld, %ld) bearing (%ld, %ld) advance_x %ld\n",
		       ch, ch,
		       metrics->width >> 6, metrics->height >> 6,
		       metrics->horiBearingX >> 6, metrics->horiBearingY >> 6,
		       metrics->horiAdvance >> 6);
	}

	hdrsz = 1 + 1 + 1 + 5 + 10;
	width = npot(width);
	height = npot(height);
	total_width = width * 128;
	total_height = height;
	data = calloc(1, total_width * total_height + hdrsz);
	if (!data)
		errx(1, "Failed to allocate memory");

	printf("dimension char (%dx%d) total (%dx%d)\n", width, height, total_width, total_height);
	hdr = data;
	img = data + hdrsz;
	hdr[2] = 3;
	hdr[12] = total_width & 0xff;
	hdr[13] = (total_width >> 8) & 0xff;
	hdr[14] = total_height & 0xff;
	hdr[15] = (total_height >> 8) & 0xff;
	hdr[16] = 8;
	hdr[17] = 0x20;

	for (ch = 32; ch <= 127; ch++) {
		FT_Load_Glyph(face, FT_Get_Char_Index(face, ch), FT_LOAD_DEFAULT);
		FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

		bmp = &face->glyph->bitmap;
		for (y = 0; y < bmp->rows; y++) {
			pix = img + y * total_width + width * (ch - 32);
			bmppix = bmp->buffer + y * bmp->width;
			for (x = 0; x < bmp->width; x++) {
				*pix++ = *bmppix++;
			}
		}
	}

	fwrite(hdr, hdrsz, 1, fp);
	fwrite(img, total_width * total_height, 1, fp);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	fclose(fp);
	free(data);

	return 0;
}
