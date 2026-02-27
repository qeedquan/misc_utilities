#ifndef _TIMERLIB_H_
#define _TIMERLIB_H_

#include "types/vxTypes.h"

typedef void (*TIMER_CONNECT_FUNC)(timer_t timerId, _Vx_usr_arg_t arg);

#define timer_create vxtimer_create

int vxtimer_create(clockid_t clockid, struct sigevent *sevp, timer_t *timerid);
int timer_connect(timer_t timerid, TIMER_CONNECT_FUNC routine, _Vx_usr_arg_t arg);

#endif
