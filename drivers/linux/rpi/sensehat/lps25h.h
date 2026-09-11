#ifndef _LPS25H_H_
#define _LPS25H_H_

enum {
	LPS25H_ADDRESS = 0x5c,
	LPS25H_ID = 0xbd,
};

enum {
	LPS25H_WHO_AM_I = 0x0f,
	LPS25H_CONF = 0x10,
	LPS25H_CTRL1 = 0x20,
	LPS25H_CTRL2 = 0x21,
	LPS25H_STATUS = 0x27,
	LPS25H_PRESS_OUT_XL = 0x28,
	LPS25H_OUT_L = 0x29,
	LPS25H_OUT_H = 0x2a,
	LPS25H_TEMP_OUT_L = 0x2b,
	LPS25H_TEMP_OUT_H = 0x2c,
	LPS25H_FIFO_CTRL = 0x2e,
};

typedef struct {
	int fd;
} lps25h_t;

int lps25hopen(lps25h_t *lh, const char *dev, int addr);
void lps25hclose(lps25h_t *lh);
int lps25hread(lps25h_t *lh, float *pressure, float *temperature);

#endif
