#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "fb.h"
#include "util.h"

int
fbdevs(char ***devs, size_t *len)
{
	return finddevs(devs, len, "/sys/class/graphics/fb*/name", "RPi-Sense FB", "/dev", 1);
}

int
fbopen(fb_t *fb, const char *dev)
{
	int fd;
	uint16_t *pix;

	fd = open(dev, O_RDWR);
	if (fd < 0)
		return -errno;

	fb->w = 8;
	fb->h = 8;
	pix = mmap(0, fb->w * fb->h * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (pix == MAP_FAILED) {
		close(fd);
		return -errno;
	}

	fb->fd = fd;
	fb->pix = pix;
	fbresetgamma(fb, FB_GAMMA_DEFAULT);

	return 0;
}

void
fbclose(fb_t *fb)
{
	fbresetgamma(fb, FB_GAMMA_DEFAULT);
	memset(fb->pix, 0, fb->w * fb->h * 2);
	munmap(fb->pix, fb->w * fb->h * 2);
	close(fb->fd);
}

uint16_t
fbrgb(uint8_t r, uint8_t g, uint8_t b)
{
	return ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3);
}

void
fbclear(fb_t *fb, uint16_t col)
{
	int x, y;

	for (y = 0; y < fb->h; y++) {
		for (x = 0; x < fb->w; x++)
			fb->pix[y * fb->w + x] = col;
	}
}

void
fbset(fb_t *fb, int x, int y, uint16_t col)
{
	fb->pix[y * fb->w + x] = col;
}

uint16_t
fbget(fb_t *fb, int x, int y)
{
	return fb->pix[y * fb->w + x];
}

int
fbgetgamma(fb_t *fb, uint8_t data[32])
{
	if (ioctl(fb->fd, FB_GET_GAMMA, data) < 0)
		return -errno;
	return 0;
}

int
fbsetgamma(fb_t *fb, uint8_t data[32])
{
	fbresetgamma(fb, FB_GAMMA_USER);
	if (ioctl(fb->fd, FB_SET_GAMMA, data) < 0)
		return -errno;
	return 0;
}

int
fbresetgamma(fb_t *fb, int cmd)
{
	if (ioctl(fb->fd, FB_RST_GAMMA, cmd) < 0)
		return -errno;
	return 0;
}
