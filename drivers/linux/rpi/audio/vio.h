#ifndef _VIO_H_
#define _VIO_H_

#include <stdint.h>

enum {
	VIO_MAP_SIZE = 16 * 1024 * 1024,
};

typedef struct {
	int fd;
	volatile void *mmio;
	volatile uint32_t *gpio;
	volatile uint32_t *pwm;
	volatile uint32_t *cm;
	volatile uint32_t *dma;
} vio_t;

uintptr_t viobase(int rev);

int vioopen(vio_t *vi, uintptr_t base);
void vioclose(vio_t *vi);

enum {
	GPIO_FSEL0 = 0x00 >> 2,

	GPIO_ALT0 = 4,
	GPIO_FUNC_MASK = 0x07,
};

void gpiosel(vio_t *vi, unsigned pin, int func);

enum {
	CM_PWMCTL = 0xa0 >> 2,
	CM_PWMDIV = 0xa4 >> 2,

	CM_PASSWORD = 0x5a000000,
	CM_DIV_MASK = 0x7ff,

	CM_ENAB = 0x10,

	CM_SRC_GND = 0,
	CM_SRC_OSC = 1,
	CM_SRC_TEST_DEBUG0 = 2,
	CM_SRC_TEST_DEBUG1 = 3,
	CM_SRC_PLLAPER = 4,
	CM_SRC_PLLCPER = 5,
	CM_SRC_PLLDPER = 6,
	CM_SRC_HDMI_AUX = 7,
};

enum {
	PWM_CTL = 0x00 >> 2,
	PWM_STA = 0x04 >> 2,
	PWM_DMAC = 0x08 >> 2,
	PWM_RNG1 = 0x10 >> 2,
	PWM_DAT1 = 0x14 >> 2,
	PWM_FIF1 = 0x18 >> 2,
	PWM_RNG2 = 0x20 >> 2,
	PWM_DAT2 = 0x24 >> 2,

	PWM_PWEN1 = 1 << 0,
	PWM_MODE1 = 1 << 1,
	PWM_RPTL1 = 1 << 2,
	PWM_SBIT1 = 1 << 3,
	PWM_POLA1 = 1 << 4,
	PWM_USEF1 = 1 << 5,
	PWM_CLRF1 = 1 << 6,
	PWM_MSEN1 = 1 << 7,
	PWM_PWEN2 = 1 << 8,
	PWM_MODE2 = 1 << 9,
	PWM_RPTL2 = 1 << 10,
	PWM_SBIT2 = 1 << 11,
	PWM_POLA2 = 1 << 12,
	PWM_USEF2 = 1 << 13,
	PWM_MSEN2 = 1 << 15,

	PWM_WERR1 = 1 << 2,
	PWM_RERR1 = 1 << 3,
	PWM_BERR = 1 << 8,
};

#define PWM_ENAB (1UL << 31)

enum {
	DMA_ENABLE = 0xff0 >> 2,

	DMA_EN0 = 1 << 0,
	DMA_EN1 = 1 << 1,
	DMA_EN2 = 1 << 2,
	DMA_EN3 = 1 << 3,
	DMA_EN4 = 1 << 4,
	DMA_EN5 = 1 << 5,
	DMA_EN6 = 1 << 6,
	DMA_EN7 = 1 << 7,
	DMA_EN8 = 1 << 8,
	DMA_EN9 = 1 << 9,
	DMA_EN10 = 1 << 10,
	DMA_EN11 = 1 << 11,
	DMA_EN12 = 1 << 12,
	DMA_EN13 = 1 << 13,
	DMA_EN14 = 1 << 14,

	DMA_ACTIVE = 1 << 0,
	DMA_END = 1 << 1,
	DMA_INT = 1 << 2,
	DMA_DREQ = 1 << 3,
	DMA_ABORT = 1 << 30,
};

#define DMA_RESET (1UL << 31)

#endif
