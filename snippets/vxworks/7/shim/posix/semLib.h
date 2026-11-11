#ifndef _SEMLIB_H_
#define _SEMLIB_H_

#include "types/vxTypes.h"
#include "msgQLib.h"
#include "semLibCommon.h"

#define SEM_KERNEL 0x100
#define SEM_USER 0x200
#define SEM_KUTYPE_DEFAULT 0x000
#define SEM_KUTYPE_MASK 0x300

SEM_ID semBCreate(int options, SEM_B_STATE initialState);
SEM_ID semMCreate(int options);

STATUS semTake(SEM_ID semId, _Vx_ticks_t timeout);
STATUS semGive(SEM_ID semId);

STATUS semDelete(SEM_ID semId);

STATUS semShow(SEM_ID semId, int level);

STATUS semMInit(SEMAPHORE *semaphore, int options);

#endif
