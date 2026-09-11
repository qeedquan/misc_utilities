#ifndef _FBDEV_H_
#define _FBDEV_H_

typedef struct fbPixelFormat {
	unsigned int flags;
	char alphaBits;
	char redBits;
	char greenBits;
	char blueBits;
} FB_PIXEL_FORMAT;

typedef struct fbInfo {
	void *pFirstFb;
	void *pFirstFbPhys;
	void *pFb;
	void *pFbPhys;
	unsigned int bpp;
	FB_PIXEL_FORMAT pixelFormat;
	unsigned int width, stride;
	unsigned int height;
	unsigned int offset;
	unsigned int vsync;
	unsigned int buffers;
} FB_INFO;

typedef struct fbConfig {
	int id;
	FB_PIXEL_FORMAT pixelFormat;
} FB_CONFIG;

typedef union fbIoctlArg {
	struct {
		char *pBuf;
	} getVideoMode;

	FB_INFO getFbInfo;
} FB_IOCTL_ARG;

enum {
	FB_IOCTL_DEV_SHOW,
	FB_IOCTL_GET_VIDEO_MODE,
	FB_IOCTL_GET_FB_INFO,
};

#endif
