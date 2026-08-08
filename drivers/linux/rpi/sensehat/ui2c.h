#ifndef _UI2C_H_
#define _UI2C_H_

#include <stdint.h>

enum {
	BSC_C = 0x0,
	BSC_S,
	BSC_DLEN,
	BSC_A,
	BSC_FIFO,
	BSC_DIV,
	BSC_DEL,
	BSC_CLKT,
};

enum {
	BSC_C_READ = 1 << 0,
	BSC_C_CLEAR = 1 << 4,
	BSC_C_ST = 1 << 7,
	BSC_C_INTD = 1 << 8,
	BSC_C_INTT = 1 << 9,
	BSC_C_INTR = 1 << 10,
	BSC_C_I2CEN = 1 << 15,
};

enum {
	BSC_S_TA = 1 << 0,
	BSC_S_DONE = 1 << 1,
	BSC_S_TXW = 1 << 2,
	BSC_S_RXR = 1 << 3,
	BSC_S_TXD = 1 << 4,
	BSC_S_RXD = 1 << 5,
	BSC_S_TXE = 1 << 6,
	BSC_S_RXF = 1 << 7,
	BSC_S_ERR = 1 << 8,
	BSC_S_CLKT = 1 << 9,
};

typedef struct {
	int fd;
	volatile uint32_t *regs;

	long timeout;
	uint8_t slave;
} ui2c_t;

int ui2copen(ui2c_t *ic, uintptr_t base);
void ui2cclose(ui2c_t *ic);

int ui2crd(ui2c_t *ic, void *buf, size_t size);
int ui2crb(ui2c_t *ic);

int ui2cwd(ui2c_t *ic, void *buf, size_t size);
int ui2cwb(ui2c_t *ic, uint8_t cmd);

#endif
