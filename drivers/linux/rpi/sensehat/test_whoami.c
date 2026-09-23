#include <stdio.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <linux/i2c-dev.h>
#include "hts221.h"
#include "lps25h.h"
#include "util.h"
#include "ui2c.h"

typedef struct {
	uint8_t addr;
	uint8_t reg;
	uint8_t id;
} wai_t;

static const wai_t wais[] = {
    {HTS221_ADDRESS, HTS221_WHO_AM_I, HTS221_ID},
    {LPS25H_ADDRESS, LPS25H_WHO_AM_I, LPS25H_ID},
};

void
i2cwai(void)
{
	int fd;
	uint8_t id;
	size_t i;

	fd = open("/dev/i2c-1", O_RDWR);
	if (fd < 0)
		err(1, "open");

	for (i = 0; i < nelem(wais); i++) {
		if (ioctl(fd, I2C_SLAVE, wais[i].addr) < 0)
			err(1, "ioctl");

		write(fd, &wais[i].reg, 1);
		read(fd, &id, 1);
		if (id != wais[i].id)
			printf("%zu: id mismatch %#x %#x\n", i, id, wais[i].id);
	}

	close(fd);
}

void
ui2cwai(void)
{
	ui2c_t ic[1];
	size_t i;
	int id;

	if (ui2copen(ic, 0x3f000000) < 0)
		err(1, "ui2copen");

	ic->timeout = 1000;
	for (i = 0; i < nelem(wais); i++) {
		ic->slave = wais[i].addr;
		ui2cwb(ic, wais[i].reg);
		id = ui2crb(ic);
		if (id != wais[i].id)
			printf("%zu: id mismatch %#x %#x\n", i, id, wais[i].id);
	}

	ui2cclose(ic);
}

int
main(void)
{
	i2cwai();
	ui2cwai();
	return 0;
}
