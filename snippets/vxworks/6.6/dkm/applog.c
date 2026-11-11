#define LOG_LOCATION LOG_LOC_FUNC

#include <stdio.h>
#include <applUtilLib.h>

void
test_applog(void)
{
	/*
	 * The log categories are hardcoded, for a catch-all use GENERAL_LOG
	 * The levels corresponds to Linux log levels from syslog
	 * */
	log_warning(GENERAL_LOG, "WARNING");
	log_err(GENERAL_LOG, "ERROR");
	log_notice(GENERAL_LOG, "NOTICE");
	loggerInfoShow();
}
