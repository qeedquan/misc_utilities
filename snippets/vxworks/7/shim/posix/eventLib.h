#ifndef _EVENTLIB_H_
#define _EVENTLIB_H_

#include "taskLibCommon.h"

STATUS eventClear(void);

STATUS eventReceive(UINT32 events, UINT8 options, _Vx_ticks_t timeout, UINT32 *pEventsReceived);
STATUS eventReceiveEx(UINT32 events, UINT32 options, _Vx_ticks_t timeout, UINT32 *pEventsReceived);

STATUS eventSend(TASK_ID taskId, UINT32 events);

#endif
