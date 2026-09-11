#ifndef _EVENTLIB_COMMON_H_
#define _EVENTLIB_COMMON_H_

#include "types/vxTypes.h"

typedef struct {
	UINT32 wanted;
	UINT32 received;
	UINT8 options;
} EVENTS_DESC;

typedef UINT32 _Vx_event_t;

#define S_eventLib_TIMEOUT (M_eventLib | 0x0001)
#define S_eventLib_NOT_ALL_EVENTS (M_eventLib | 0x0002)
#define S_eventLib_ALREADY_REGISTERED (M_eventLib | 0x0003)
#define S_eventLib_EVENTSEND_FAILED (M_eventLib | 0x0004)
#define S_eventLib_ZERO_EVENTS (M_eventLib | 0x0005)
#define S_eventLib_TASK_NOT_REGISTERED (M_eventLib | 0x0006)
#define S_eventLib_NULL_TASKID_AT_INT_LEVEL (M_eventLib | 0x0007)
#define S_eventLib_INVALID_OPTIONS (M_eventLib | 0x0008)

#define EVENTS_WAIT_ALL 0x00
#define EVENTS_WAIT_ANY 0x01
#define EVENTS_RETURN_ALL 0x02
#define EVENTS_KEEP_UNWANTED 0x04
#define EVENTS_FETCH 0x80

#define EVENTS_OPTIONS_NONE 0x00
#define EVENTS_SEND_ONCE 0x01
#define EVENTS_ALLOW_OVERWRITE 0x02
#define EVENTS_SEND_IF_FREE 0x04

#define EVENTS_Q_INTERRUPTIBLE 0x10000
#define EVENTS_TASK_DELETION_WAKEUP 0x20000

#endif
