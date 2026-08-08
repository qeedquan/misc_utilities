#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <syslog.h>

typedef struct {
	char ident[32];
	int logopt;
	int facility;
} Option;

Option opt = {
	.ident = "Syslog Demo",
	.logopt = LOG_PID | LOG_CONS,
	.facility = LOG_USER | LOG_DEBUG | LOG_ERR,
};

int
main(int argc, char *argv[])
{
	int i, n;

	n = 10;
	if (argc >= 2)
		n = atoi(argv[1]);

	openlog(opt.ident, opt.logopt, opt.facility);
	for (i = 0; i < n; i++)
		syslog(LOG_DEBUG, "From syslog %d", i);

	return 0;
}
