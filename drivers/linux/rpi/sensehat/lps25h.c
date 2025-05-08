// https://www.st.com/resource/en/datasheet/lps25h.pdf
// based on https://github.com/RPi-Distro/RTIMULib
// pressure/temperature sensor

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include "lps25h.h"
#include "util.h"

int
lps25hopen(lps25h_t *lh, const char *dev, int addr)
{
	static const struct {
		int addr;
		int value;
	} init[] = {
	    {LPS25H_CTRL1, 0xc4},
	    {LPS25H_CONF, 0x05},
	    {LPS25H_FIFO_CTRL, 0xc0},
	    {LPS25H_CTRL2, 0x40},
	    {0, 0},
	};

	int fd, rv;
	size_t i;

	fd = open(dev, O_RDWR);
	if (fd < 0)
		return -errno;

	if (ioctl(fd, I2C_SLAVE, addr) < 0)
		goto error;

	for (i = 0; init[i].addr; i++) {
		if (i2c_smbus_write_byte_data(fd, init[i].addr, init[i].value) < 0)
			goto error;
	}

	lh->fd = fd;
	return 0;

error:
	rv = -errno;
	if (fd >= 0)
		close(fd);
	return rv;
}

void
lps25hclose(lps25h_t *lh)
{
	close(lh->fd);
}

int
lps25hread(lps25h_t *lh, float *pressure, float *temperature)
{
	unsigned char data[3];
	int status;

	*pressure = 0;
	*temperature = 0;

	if ((status = i2c_smbus_read_byte_data(lh->fd, LPS25H_STATUS)) < 0)
		return -1;

	if (status & 2) {
		if (i2c_smbus_read_i2c_block_data(lh->fd, LPS25H_PRESS_OUT_XL + 0x80, 3, data) != 3)
			return -1;

		*pressure = (data[2] << 16 | data[1] << 8 | data[0]) / 4096.0f;
	}

	if (status & 1) {
		if (i2c_smbus_read_i2c_block_data(lh->fd, LPS25H_TEMP_OUT_L + 0x80, 2, data) != 2)
			return -1;

		*temperature = seload(data[1] << 8 | data[0], 16) / 480.0f + 42.5f;
	}

	return status & 0x3;
}
