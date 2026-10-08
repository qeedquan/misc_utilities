#ifndef _TASK_PXLIBP_H_
#define _TASK_PXLIBP_H_

#include "errnoLib.h"
#include "types/vxTypes.h"
#include "taskLibCommon.h"
#include <time.h>

typedef struct {
	UINT flag;
	int priority;
	UINT32 policy;

	int ssLowPrio;
	struct timespec ssReplPeriod;
	struct timespec ssInitBudget;
	int ssMaxRepl;
} TASK_PX_ATTR;

STATUS taskPxAttrGet(TASK_ID tid, TASK_PX_ATTR *pAttr);

#endif
