#include <stdio.h>
#include <string.h>
#include <errno.h>

void
decode_errno(int start, int end)
{
	int err;

	// strerror contains more information about error codes in kernel space vs user space
	for (err = start; err <= end; err++)
		printf("%#x: %s\n", err, strerror(err));
}
