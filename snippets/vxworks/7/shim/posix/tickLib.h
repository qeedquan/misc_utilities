#ifndef _TICKLIB_H_
#define _TICKLIB_H_

#include "types/vxTypes.h"
#include "errnoLib.h"

_Vx_ticks_t tickGet(void);
_Vx_ticks64_t tick64Get(void);

_Vx_freq_t tickRateGet(void);

uint64_t timespec_to_millisecond(struct timespec *tp);
struct timespec millisecond_to_timespec(uint64_t ms);

uint64_t tick_to_millisecond(_Vx_ticks64_t tick);

#endif
