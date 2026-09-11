#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "ui2c.h"
#include "util.h"

static int
xfer(ui2c_t *ic, void *buf, size_t size, int op)
{
	uint32_t c, s;
	char *p;
	size_t n;
	long t;

	if (size == 0)
		return 0;

	if (size > 0xffff)
		return -E2BIG;

	ic->regs[BSC_A] = ic->slave;
	ic->regs[BSC_DLEN] = size;

	c = BSC_C_I2CEN | BSC_C_ST;
	if (op == 'r')
		c |= BSC_C_READ;
	ic->regs[BSC_C] = c;

	p = buf;
	n = size;
	t = ic->timeout;
	for (;;) {
		s = ic->regs[BSC_S];
		if (s & (BSC_S_CLKT | BSC_S_ERR)) {
			ic->regs[BSC_C] = 0;
			return -s;
		}

		if (t <= 0) {
			ic->regs[BSC_C] = 0;
			return -ETIMEDOUT;
		}

		switch (op) {
		case 'r':
			while (n && ic->regs[BSC_S] & BSC_S_RXD) {
				*p = ic->regs[BSC_FIFO];
				p++;
				n--;
			}
			break;

		case 'w':
			while (n && ic->regs[BSC_S] & BSC_S_TXD) {
				ic->regs[BSC_FIFO] = *p;
				p++;
				n--;
			}
			break;
		}

		if (s & BSC_S_DONE)
			break;

		t -= 10;
		millisleep(10);
	}

	ic->regs[BSC_C] = BSC_C_CLEAR;
	ic->regs[BSC_S] = BSC_S_CLKT | BSC_S_ERR | BSC_S_DONE;

	return size;
}

int
ui2copen(ui2c_t *ic, uintptr_t base)
{
	int fd, flags, pagesize;
	volatile uint32_t *regs;

	pagesize = getpagesize();
	flags = O_RDWR | O_SYNC | O_CLOEXEC;
	fd = open("/dev/mem", flags);
	if (fd < 0)
		goto err;

	regs = mmap(NULL, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, base + 0x804000);
	if (regs == MAP_FAILED)
		goto err;

	ic->fd = fd;
	ic->regs = regs;

	return 0;

err:
	if (regs)
		munmap((void *)regs, pagesize);
	if (fd >= 0)
		close(fd);
	return -errno;
}

void
ui2cclose(ui2c_t *ic)
{
	munmap((void *)ic->regs, getpagesize());
	close(ic->fd);
}

int
ui2crd(ui2c_t *ic, void *buf, size_t size)
{
	return xfer(ic, buf, size, 'r');
}

int
ui2crb(ui2c_t *ic)
{
	uint8_t cmd;
	int ret;

	ret = ui2crd(ic, &cmd, sizeof(cmd));
	if (ret < 0)
		return ret;
	return cmd;
}

int
ui2cwd(ui2c_t *ic, void *buf, size_t size)
{
	return xfer(ic, buf, size, 'w');
}

int
ui2cwb(ui2c_t *ic, uint8_t cmd)
{
	return ui2cwd(ic, &cmd, sizeof(cmd));
}
