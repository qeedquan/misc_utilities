#include <vxWorks.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

const char *
sysNvRamPath()
{
	return "/eeprom/0";
}

int
sysNvRamSize()
{
	return 0x100000;
}

STATUS
sysNvRamGet(char *string, int length, int offset)
{
	STATUS status;
	int size;
	int fd;
	int bytes;

	size = sysNvRamSize();
	if (offset < 0 || length < 0 || offset > size || length > size || offset + length > size)
		return ERROR;

	status = OK;
	fd = open(sysNvRamPath(), O_RDONLY);
	if (fd < 0)
		goto error;

	if (lseek(fd, offset, SEEK_SET) != offset)
		goto error;

	bytes = read(fd, string, length);
	if (bytes < 0)
		goto error;

	string[bytes] = 0;

	if (0) {
	error:
		status = ERROR;
	}

	if (fd >= 0)
		close(fd);

	return status;
}

STATUS
sysNvRamSet(char *string, int length, int offset)
{
	STATUS status;
	int size;
	int fd;
	int bytes;
	int i;

	size = sysNvRamSize();
	if (offset < 0 || length < 0 || offset > size || length > size || offset + length > size)
		return ERROR;

	status = OK;
	fd = open(sysNvRamPath(), O_RDWR);
	if (fd < 0)
		goto error;

	if (lseek(fd, offset, SEEK_SET) != offset)
		goto error;

	// workaround for the limitation of our i2c driver where it only handles writing a few bytes per transfer
	for (i = 0; i < length; i++) {
		bytes = write(fd, string + i, 1);
		if (bytes != 1)
			goto error;
	}

	if (0) {
	error:
		status = ERROR;
	}

	if (fd >= 0)
		close(fd);

	return status;
}

void
nvramlink()
{
}
