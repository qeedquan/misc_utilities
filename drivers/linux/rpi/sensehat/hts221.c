// https://www.st.com/resource/en/datasheet/hts221.pdf
// based on https://github.com/RPi-Distro/RTIMULib
// humidity/temperature sensor

#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include "hts221.h"
#include "util.h"

int
hts221open(hts221_t *hs, const char *dev, int addr)
{
	static const struct {
		int addr;
		int value;
	} init[] = {
	    {HTS221_CTRL1, 0x87},
	    {HTS221_AV_CONF, 0x1b},
	    {0, 0},
	};

	int fd, rv;
	size_t i;

	unsigned char data[2];
	uint8_t H0_H_2 = 0;
	uint8_t H1_H_2 = 0;
	uint16_t T0_C_8 = 0;
	uint16_t T1_C_8 = 0;
	int16_t H0_T0_OUT = 0;
	int16_t H1_T0_OUT = 0;
	int16_t T0_OUT = 0;
	int16_t T1_OUT = 0;
	float H0, H1, T0, T1;

	fd = open(dev, O_RDWR);
	if (fd < 0)
		return -errno;

	if (ioctl(fd, I2C_SLAVE, addr) < 0)
		goto error;

	for (i = 0; init[i].addr; i++) {
		if (i2c_smbus_write_byte_data(fd, init[i].addr, init[i].value) < 0)
			goto error;
	}

	rv = 0;
	rv += i2c_smbus_read_i2c_block_data(fd, HTS221_T1_T0 + 0x80, 1, &data[1]);
	rv += i2c_smbus_read_i2c_block_data(fd, HTS221_T0_C_8 + 0x80, 1, &data[0]);
	T0_C_8 = (data[1] & 0x3) << 8 | data[0];
	T0 = T0_C_8 / 8.0f;

	rv += i2c_smbus_read_i2c_block_data(fd, HTS221_T1_C_8 + 0x80, 1, &data[0]);
	T1_C_8 = ((data[1] & 0xc) << 6) | data[0];
	T1 = T1_C_8 / 8.0f;

	rv += i2c_smbus_read_i2c_block_data(fd, HTS221_T0_OUT + 0x80, 2, &data[0]);
	T0_OUT = seload(data[1] << 8 | data[0], 16);

	rv += i2c_smbus_read_i2c_block_data(fd, HTS221_T1_OUT + 0x80, 2, &data[0]);
	T1_OUT = seload(data[1] << 8 | data[0], 16);

	rv += i2c_smbus_read_i2c_block_data(fd, HTS221_H0_H_2 + 0x80, 1, &H0_H_2);
	H0 = H0_H_2 / 2.0f;

	rv += i2c_smbus_read_i2c_block_data(fd, HTS221_H1_H_2 + 0x80, 1, &H1_H_2);
	H1 = H1_H_2 / 2.0f;

	rv += i2c_smbus_read_i2c_block_data(fd, HTS221_H0_T0_OUT + 0x80, 2, &data[0]);
	H0_T0_OUT = seload(data[1] << 8 | data[0], 16);

	rv += i2c_smbus_read_i2c_block_data(fd, HTS221_H1_T0_OUT + 0x80, 2, &data[0]);
	H1_T0_OUT = seload(data[1] << 8 | data[0], 16);

	if (rv != 1 + 1 + 1 + 2 + 2 + 1 + 1 + 2 + 2)
		goto error;

	hs->tm = (T1 - T0) / (T1_OUT - T0_OUT);
	hs->tc = T0 - (hs->tm * T0_OUT);
	hs->hm = (H1 - H0) / (H1_T0_OUT - H0_T0_OUT);
	hs->hc = H0 - (hs->hm * H0_T0_OUT);

	hs->fd = fd;
	return 0;

error:
	rv = -errno;
	if (fd >= 0)
		close(fd);
	return rv;
}

void
hts221close(hts221_t *hs)
{
	close(hs->fd);
}

int
hts221read(hts221_t *hs, float *humidity, float *temperature)
{
	unsigned char data[3];
	int status;

	*humidity = 0;
	*temperature = 0;

	if ((status = i2c_smbus_read_byte_data(hs->fd, HTS221_STATUS)) < 0)
		return -1;

	if (status & 2) {
		if (i2c_smbus_read_i2c_block_data(hs->fd, HTS221_HUMIDITY_OUT_L + 0x80, 2, data) != 2)
			return -1;

		*humidity = seload(data[1] << 8 | data[0], 16);
		*humidity = *humidity * hs->hm + hs->hc;
	}

	if (status & 1) {
		if (i2c_smbus_read_i2c_block_data(hs->fd, HTS221_TEMP_OUT_L + 0x80, 2, data) != 2)
			return -1;

		*temperature = seload(data[1] << 8 | data[0], 16);
		*temperature = *temperature * hs->tm + hs->tc;
	}

	return status & 0x3;
}
