#include "vxWorks.h"

void
spinLockTaskInit(spinlockTask_t *lock, int flags)
{
	lock->flags = flags;
}
