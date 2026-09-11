#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>

// call this before using syslog() in rtps to redirect the output
void
redirect_syslog(void)
{
	int fd;

	fd = open("/ram0/my_syslog.txt", O_RDWR | O_CREAT);
	if (fd >= 0)
		syslogFdSet(fd);
}
