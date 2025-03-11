#include <stdio.h>
#include <string.h>
#include <time.h>
#include <err.h>
#include "lps25h.h"
#include "hts221.h"
#include "lsm9ds1.h"
#include "util.h"

lps25h_t lh[1];
hts221_t hs[1];
lsm9ds1_t lm[1];

int
main(void)
{
	const char *device;
	float humidity, pressure, temperature;
	float gyro[3], accel[3], mag[3];
	int rv;

	device = "/dev/i2c-1";
	if ((rv = lps25hopen(lh, device, LPS25H_ADDRESS)) < 0)
		errx(1, "lps25h: failed to open device %s: %s", device, strerror(-rv));

	if ((rv = hts221open(hs, device, HTS221_ADDRESS)) < 0)
		errx(1, "hts221: failed to open device %s: %s", device, strerror(-rv));

	if ((rv = lsm9ds1open(lm, device, LSM9DS1_ADDRESS0)) < 0)
		errx(1, "lsm9ds1: failed to open device %s: %s", device, strerror(-rv));

	for (;;) {
		rv = lps25hread(lh, &pressure, &temperature);
		printf("status %d pressure %f temperature %f\n", rv, pressure, temperature);

		rv = hts221read(hs, &humidity, &temperature);
		printf("status %d humidity %f temperature %f\n", rv, humidity, temperature);

		rv = lsm9ds1read(lm, gyro, accel, mag);
		printf("status %d gyro [%f %f %f] accel [%f %f %f] mag [%f %f %f] vmag %f\n",
		       rv, gyro[0], gyro[1], gyro[2], accel[0], accel[1], accel[2], mag[0], mag[1], mag[2], vmag(mag));

		// if we read too fast, the sensor won't be ready and will return not ready status
		// so we need to delay between the reads for valid data
		millisleep(100);
	}
	lps25hclose(lh);
	hts221close(hs);

	return 0;
}
