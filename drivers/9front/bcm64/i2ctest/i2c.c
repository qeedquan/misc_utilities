#include <u.h>
#include <libc.h>
#include "i2c.h"

int
i2copen(I2C *ic, int dev)
{
	char name[64];
	int cfd, dfd;

	snprint(name, sizeof(name), "/dev/i2c%dctl", dev);
	cfd = open(name, ORDWR);

	snprint(name, sizeof(name), "/dev/i2c%ddata", dev);
	dfd = open(name, ORDWR);

	if (cfd < 0 || dfd < 0) {
		if (cfd >= 0)
			close(cfd);
		if (dfd >= 0)
			close(dfd);

		werrstr("i2c: failed to open device");
		return -1;
	}
	fprint(cfd, "enable");

	ic->cfd = cfd;
	ic->dfd = dfd;
	return 0;
}

int
i2cclose(I2C *ic)
{
	int rv;

	rv = close(ic->cfd);
	rv |= close(ic->dfd);
	return rv;
}

int
i2csetslave(I2C *ic, int slave)
{
	if (fprint(ic->cfd, "slave %d", slave) < 0)
		return -1;
	return 0;
}

int
i2csmbwd(I2C *ic, void *a, int n)
{
	return write(ic->dfd, a, n);
}

int
i2csmbwb(I2C *ic, u8int v)
{
	return i2csmbwd(ic, &v, sizeof(v));
}

int
i2csmbrd(I2C *ic, u8int r, void *a, int n)
{
	if (i2csmbwb(ic, r) < 0)
		return -1;
	return read(ic->dfd, a, n);
}

int
i2csmbrb(I2C *ic, u8int r)
{
	u8int v;

	if (i2csmbrd(ic, r, &v, sizeof(v)) < 0)
		return -1;
	return v;
}
