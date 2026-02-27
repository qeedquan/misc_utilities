#ifndef _FB_H_
#define _FB_H_

#include <stdint.h>

enum {
	FB_GET_GAMMA = 0xf100,
	FB_SET_GAMMA = 0xf101,
	FB_RST_GAMMA = 0xf102,
};

enum {
	FB_GAMMA_DEFAULT = 0,
	FB_GAMMA_LOW,
	FB_GAMMA_USER
};

typedef struct {
	int fd;
	int w, h;
	uint16_t *pix;
} fb_t;

int fbdevs(char ***devs, size_t *len);
int fbopen(fb_t *fb, const char *dev);
void fbclose(fb_t *fb);

void fbclear(fb_t *fb, uint16_t col);
void fbset(fb_t *fb, int x, int y, uint16_t col);
uint16_t fbget(fb_t *fb, int x, int y);

uint16_t fbrgb(uint8_t r, uint8_t g, uint8_t b);

int fbgetgamma(fb_t *fb, uint8_t data[32]);
int fbsetgamma(fb_t *fb, uint8_t data[32]);
int fbresetgamma(fb_t *fb, int cmd);

#endif
