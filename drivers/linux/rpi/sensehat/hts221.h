#ifndef _HTS221_H_
#define _HTS221_H_

#include <stdint.h>

enum {
	HTS221_ADDRESS = 0x5f,
	HTS221_ID = 0xbc,
};

enum {
	HTS221_WHO_AM_I = 0x0f,
	HTS221_AV_CONF = 0x10,
	HTS221_CTRL1 = 0x20,
	HTS221_CTRL2 = 0x21,
	HTS221_CTRL3 = 0x22,
	HTS221_STATUS = 0x27,
	HTS221_HUMIDITY_OUT_L = 0x28,
	HTS221_HUMIDITY_OUT_H = 0x29,
	HTS221_TEMP_OUT_L = 0x2a,
	HTS221_TEMP_OUT_H = 0x2b,
	HTS221_H0_H_2 = 0x30,
	HTS221_H1_H_2 = 0x31,
	HTS221_T0_C_8 = 0x32,
	HTS221_T1_C_8 = 0x33,
	HTS221_T1_T0 = 0x35,
	HTS221_H0_T0_OUT = 0x36,
	HTS221_H1_T0_OUT = 0x3a,
	HTS221_T0_OUT = 0x3c,
	HTS221_T1_OUT = 0x3e,
};

typedef struct {
	int fd;
	float tm, tc;
	float hm, hc;
} hts221_t;

int hts221open(hts221_t *hs, const char *dev, int addr);
void hts221close(hts221_t *hs);
int hts221read(hts221_t *hs, float *humidity, float *temperature);

#endif
