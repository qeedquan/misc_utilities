#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "vio.h"
#include "util.h"

uintptr_t
viobase(int rev)
{
	if (rev == 1)
		return 0x20000000;
	return 0x3f000000;
}

int
vioopen(vio_t *vi, uintptr_t base)
{
	volatile void *mmio;
	int fd;

	mmio = MAP_FAILED;
	fd = open("/dev/mem", O_RDWR);
	if (fd < 0)
		goto err;

	mmio = mmap(NULL, pgroundup(VIO_MAP_SIZE), PROT_READ | PROT_WRITE, MAP_SHARED, fd, base);
	if (mmio == MAP_FAILED)
		goto err;

	vi->mmio = mmio;
	vi->gpio = (volatile uint32_t *)((uintptr_t)mmio + 0x200000);
	vi->pwm = (volatile uint32_t *)((uintptr_t)mmio + 0x20c000);
	vi->cm = (volatile uint32_t *)((uintptr_t)mmio + 0x101000);
	vi->dma = (volatile uint32_t *)((uintptr_t)mmio + 0x7000);
	return 0;

err:
	if (fd >= 0)
		close(fd);
	if (mmio != MAP_FAILED)
		munmap((void *)mmio, pgroundup(VIO_MAP_SIZE));
	return -errno;
}

void
vioclose(vio_t *vi)
{
	munmap((void *)vi->mmio, pgroundup(VIO_MAP_SIZE));
	close(vi->fd);
}

void
gpiosel(vio_t *vi, unsigned pin, int func)
{
	volatile uint32_t *reg;
	int shift;

	assert(pin < 54);
	shift = (pin % 10) * 3;
	reg = &vi->gpio[GPIO_FSEL0 + pin / 10];
	func &= GPIO_FUNC_MASK;
	*reg = (*reg & ~(GPIO_FUNC_MASK << shift)) | (func << shift);
}
