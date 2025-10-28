#include "vxWorks.h"

STATUS
eventClear(void)
{
	return OK;
}

STATUS
eventReceive(UINT32 events, UINT8 options, _Vx_ticks_t timeout, UINT32 *pEventsReceived)
{
	return ERROR;
}

STATUS
eventReceiveEx(UINT32 events, UINT32 options, _Vx_ticks_t timeout, UINT32 *pEventsReceived)
{
	return ERROR;
}

STATUS
eventSend(TASK_ID taskId, UINT32 events)
{
	return ERROR;
}
