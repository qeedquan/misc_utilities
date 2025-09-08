#ifndef _ERRNOLIB_H_
#define _ERRNOLIB_H_

#include <errno.h>
#include "vwModNum.h"

#define ERROR -1
#define OK 0

#define EDOOM 39

int errnoGet(void);
void errnoSet(int code);

#endif
