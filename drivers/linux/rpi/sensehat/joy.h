#ifndef _JOY_H_
#define _JOY_H_

#include <stddef.h>

enum {
	JUP = 1 << 0,
	JDOWN = 1 << 1,
	JLEFT = 1 << 2,
	JRIGHT = 1 << 3,
	JENTER = 1 << 4,

	JPRESSED = 1 << 5,
	JRELEASED = 1 << 6,
	JHELD = 1 << 7,
};

typedef struct {
	int fd;
} joy_t;

int joydevs(char ***devs, size_t *len);
int joyopen(joy_t *joy, const char *dev);
int32_t joyread(joy_t *joy);
void joyclose(joy_t *joy);

#endif
