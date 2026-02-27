#ifndef _LOGLIB_H_
#define _LOGLIB_H_

int logMsg(char *fmt, ...);

extern int (*_func_logMsg)(char *, ...);

#endif
