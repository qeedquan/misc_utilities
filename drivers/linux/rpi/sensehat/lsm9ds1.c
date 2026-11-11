// https://www.st.com/resource/en/datasheet/lsm9ds1.pdf
// based on https://github.com/RPi-Distro/RTIMULib
// imu sensor (gyrometer + accelerometer + magnetometer)

#include <assert.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include "lsm9ds1.h"
#include "util.h"

static int
setgyro(lsm9ds1_t *lm, int rate, int bw, int fsr, int hpf)
{
	int ctrl;
	int rv;

	switch (rate) {
	case LSM9DS1_GYRO_SAMPLERATE_14_9:
		ctrl = 0x20;
		break;
	case LSM9DS1_GYRO_SAMPLERATE_59_5:
		ctrl = 0x40;
		break;
	case LSM9DS1_GYRO_SAMPLERATE_119:
		ctrl = 0x60;
		break;
	case LSM9DS1_GYRO_SAMPLERATE_238:
		ctrl = 0x80;
		break;
	case LSM9DS1_GYRO_SAMPLERATE_476:
		ctrl = 0xa0;
		break;
	case LSM9DS1_GYRO_SAMPLERATE_952:
		ctrl = 0xc0;
		break;
	default:
		assert(0);
	}

	switch (bw) {
	case LSM9DS1_GYRO_BANDWIDTH_0:
		break;
	case LSM9DS1_GYRO_BANDWIDTH_1:
		ctrl |= 0x01;
		break;
	case LSM9DS1_GYRO_BANDWIDTH_2:
		ctrl |= 0x02;
		break;
	case LSM9DS1_GYRO_BANDWIDTH_3:
		ctrl |= 0x03;
		break;
	default:
		assert(0);
	}

	// mdps/lsb (milli degrees per second per lsb)
	// so scale factor is mdps/1000 to get degrees
	switch (fsr) {
	case LSM9DS1_GYRO_FSR_250:
		lm->gs = (8.75f / 1000.0f) * DEG2RAD;
		break;
	case LSM9DS1_GYRO_FSR_500:
		lm->gs = (17.5f / 1000.0f) * DEG2RAD;
		ctrl |= 0x08;
		break;
	case LSM9DS1_GYRO_FSR_2000:
		lm->gs = (70.0f / 1000.0f) * DEG2RAD;
		ctrl |= 0x18;
		break;
	default:
		assert(0);
	}

	rv = i2c_smbus_write_byte_data(lm->fd, LSM9DS1_CTRL1, ctrl);
	rv |= i2c_smbus_write_byte_data(lm->fd, LSM9DS1_CTRL3, hpf | 0x40);
	return rv;
}

static int
setaccel(lsm9ds1_t *lm, int rate, int lpf, int fsr)
{
	int ctrl;
	int rv;

	ctrl = (rate << 5) | (fsr << 3) | lpf;

	// mG/lsb (milli G per lsb)
	// so scale factor mG/1000 to get G
	switch (fsr) {
	case LSM9DS1_ACCEL_FSR_2:
		lm->as = 0.061f / 1000.0f;
		break;
	case LSM9DS1_ACCEL_FSR_4:
		lm->as = 0.122f / 1000.0f;
		break;
	case LSM9DS1_ACCEL_FSR_8:
		lm->as = 0.244f / 1000.0f;
		break;
	case LSM9DS1_ACCEL_FSR_16:
		lm->as = 0.732f / 1000.0f;
		break;
	default:
		assert(0);
	}

	rv = i2c_smbus_write_byte_data(lm->fd, LSM9DS1_CTRL6, ctrl);
	rv |= i2c_smbus_write_byte_data(lm->fd, LSM9DS1_CTRL7, 0x00);
	return rv;
}

static int
setmag(lsm9ds1_t *lm, int rate, int fsr)
{
	int ctrl1, ctrl2;
	int rv;

	ctrl1 = rate << 2;

	// mGs (milli gauss per lsb)
	// so scale factor mG/10 to get uT (micro telsa)
	switch (fsr) {
	case LSM9DS1_COMPASS_FSR_4:
		lm->ms = (0.14f / 10.0f);
		ctrl2 = 0x00;
		break;
	case LSM9DS1_COMPASS_FSR_8:
		lm->ms = (0.29f / 10.0f);
		ctrl2 = 0x20;
		break;
	case LSM9DS1_COMPASS_FSR_12:
		lm->ms = (0.43f / 10.0f);
		ctrl2 = 0x40;
		break;
	case LSM9DS1_COMPASS_FSR_16:
		lm->ms = (0.58f / 10.0f);
		ctrl2 = 0x60;
		break;
	default:
		assert(0);
	}

	rv = i2c_smbus_write_byte_data(lm->fd, LSM9DS1_MAG_CTRL1, ctrl1);
	rv |= i2c_smbus_write_byte_data(lm->fd, LSM9DS1_MAG_CTRL2, ctrl2);
	rv |= i2c_smbus_write_byte_data(lm->fd, LSM9DS1_MAG_CTRL3, 0x00);
	return rv;
}

int
lsm9ds1open(lsm9ds1_t *lm, const char *dev, int addr)
{
	static const int magaddrs[] = {
	    LSM9DS1_MAG_ADDRESS0,
	    LSM9DS1_MAG_ADDRESS1,
	    LSM9DS1_MAG_ADDRESS2,
	    LSM9DS1_MAG_ADDRESS3,
	    0,
	};
	static const struct {
		int addr;
		int value;
	} init[] = {
	    {LSM9DS1_CTRL8, 0x80},
	    {0, 0},
	};

	int fd, rv, magaddr;
	size_t i;

	magaddr = queryi2cdev(dev, magaddrs, LSM9DS1_MAG_WHO_AM_I, LSM9DS1_MAG_ID);
	if (magaddr < 0)
		return -EINVAL;

	fd = open(dev, O_RDWR);
	if (fd < 0)
		return -errno;

	if (ioctl(fd, I2C_SLAVE, addr) < 0)
		goto syserror;

	for (i = 0; init[i].addr; i++) {
		if (i2c_smbus_write_byte_data(fd, init[i].addr, init[i].value) < 0)
			goto syserror;
	}

	millisleep(100);

	if (i2c_smbus_read_byte_data(fd, LSM9DS1_WHO_AM_I) != LSM9DS1_ID) {
		rv = -EINVAL;
		goto error;
	}

	lm->magaddr = magaddr;
	lm->agaddr = addr;
	lm->fd = fd;
	for (i = 0; i < 3; i++) {
		lm->gco[i] = 0;
		lm->gcs[i] = 1;

		lm->aco[i] = 0;
		lm->acs[i] = 1;

		lm->mco[i] = 0;
		lm->mcs[i] = 1;
	}

	if (setgyro(lm, LSM9DS1_GYRO_SAMPLERATE_119, LSM9DS1_GYRO_BANDWIDTH_1, LSM9DS1_GYRO_FSR_500, LSM9DS1_GYRO_HPF_4) < 0)
		goto syserror;

	if (setaccel(lm, LSM9DS1_ACCEL_SAMPLERATE_119, LSM9DS1_ACCEL_LPF_50, LSM9DS1_ACCEL_FSR_8) < 0)
		goto syserror;

	if (ioctl(fd, I2C_SLAVE, magaddr) < 0)
		goto syserror;

	if (setmag(lm, LSM9DS1_COMPASS_SAMPLERATE_20, LSM9DS1_COMPASS_FSR_4) < 0)
		goto syserror;

	return 0;

syserror:
	rv = -errno;
error:
	if (fd >= 0)
		close(fd);
	return rv;
}

void
lsm9ds1close(lsm9ds1_t *lm)
{
	close(lm->fd);
}

int
lsm9ds1read(lsm9ds1_t *lm, float gyro[3], float accel[3], float mag[3])
{
	unsigned char data[3][6];
	int status;
	int i, nr;

	if (ioctl(lm->fd, I2C_SLAVE, lm->agaddr) < 0)
		return -errno;
	if ((status = i2c_smbus_read_byte_data(lm->fd, LSM9DS1_STATUS)) < 0)
		return -errno;
	if (!(status & 0x2))
		return -1;

	nr = 0;
	for (i = 0; i < 6; i++) {
		nr += i2c_smbus_read_i2c_block_data(lm->fd, LSM9DS1_OUT_X_L_G + i, 1, &data[0][i]);
		nr += i2c_smbus_read_i2c_block_data(lm->fd, LSM9DS1_OUT_X_L_XL + i, 1, &data[1][i]);
	}

	if (ioctl(lm->fd, I2C_SLAVE, lm->magaddr) < 0)
		return -errno;
	for (i = 0; i < 6; i++)
		nr += i2c_smbus_read_i2c_block_data(lm->fd, LSM9DS1_MAG_OUT_X_L + i, 1, &data[2][i]);
	if (nr != 6 + 6 + 6)
		return -1;

	// read data and apply scale factors
	for (i = 0; i < 3; i++) {
		gyro[i] = seload(data[0][2 * i + 1] << 8 | data[0][2 * i], 16) * lm->gs;
		accel[i] = seload(data[1][2 * i + 1] << 8 | data[1][2 * i], 16) * lm->as;
		mag[i] = seload(data[2][2 * i + 1] << 8 | data[2][2 * i], 16) * lm->ms;
	}

	// get it into the frame convention of rtimulib
	gyro[2] = -gyro[2];

	accel[0] = -accel[0];
	accel[1] = -accel[1];

	mag[0] = -mag[0];
	mag[2] = -mag[2];

	// apply calibration
	for (i = 0; i < 3; i++) {
		gyro[i] = (gyro[i] - lm->gco[i]) * lm->gcs[i];
		accel[i] = (accel[i] - lm->aco[i]) * lm->acs[i];
		mag[i] = (mag[i] - lm->mco[i]) * lm->mcs[i];
	}

	return 0x7;
}
