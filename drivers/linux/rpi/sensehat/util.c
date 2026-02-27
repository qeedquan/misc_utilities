#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <glob.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <i2c/smbus.h>
#include <unistd.h>
#include "util.h"

int64_t
seload(uint64_t measured, size_t size)
{
	uint64_t mask;

	mask = 1ULL << (size - 1);
	return (measured & (mask - 1)) - (measured & mask);
}

uint64_t
leload(uint8_t *buf, size_t size)
{
	size_t i;
	uint64_t r;

	r = 0;
	for (i = 0; i < size; i++)
		r |= (uint64_t)buf[i] << (8 * i);
	return r;
}

int
finddevs(char ***devs, size_t *len, const char *pat, const char *sig, const char *devpath, int ndp)
{
	glob_t gl;
	size_t i;
	int j;
	FILE *fp;
	char buf[256], *name;

	if (glob(pat, 0, NULL, &gl) != 0)
		return -1;

	*devs = calloc(gl.gl_pathc, sizeof(*devs));
	*len = 0;
	if (*devs) {
		for (i = 0; i < gl.gl_pathc; i++) {
			fp = fopen(gl.gl_pathv[i], "rb");
			if (!fp)
				continue;

			if (fgets(buf, sizeof(buf), fp) && !strncmp(buf, sig, strlen(sig))) {
				name = gl.gl_pathv[i];
				for (j = 0; j < ndp; j++)
					name = dirname(name);
				name = basename(name);

				if (asprintf(&(*devs[*len]), "%s/%s", devpath, name) > 0)
					(*len)++;
			}

			fclose(fp);
		}
	}

	globfree(&gl);

	if (*len == 0)
		return -1;
	return 0;
}

int
queryi2cdev(const char *dev, const int *addrs, int reg, int id)
{
	int fd, addr;
	size_t i;

	fd = open(dev, O_RDONLY);
	if (fd < 0)
		return -errno;

	addr = -1;
	for (i = 0; addrs[i]; i++) {
		if (ioctl(fd, I2C_SLAVE, addrs[i]) < 0)
			continue;

		if (i2c_smbus_read_byte_data(fd, reg) == id) {
			addr = addrs[i];
			break;
		}
	}

	close(fd);
	return addr;
}

void
millisleep(long ms)
{
	struct timespec tp;

	tp.tv_sec = ms / 1000;
	tp.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&tp, NULL);
}

float
vmag(float v[3])
{
	return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}
