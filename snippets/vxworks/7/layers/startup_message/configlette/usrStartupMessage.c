#include <vxWorks.h>
#include <syslog.h>

void
usrStartupMessage(void)
{
	syslog(LOG_INFO, "STARTUP VERSION: %s VALUE1: %d VALUE2: %d",
	       STARTUP_VERSION, STARTUP_VALUE1, STARTUP_VALUE2);
}

// one way of exposing the config parameters to the system
const char *startup_version = STARTUP_VERSION;
int startup_value1 = STARTUP_VALUE1;
int startup_value2 = STARTUP_VALUE2;
