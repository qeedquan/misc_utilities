#ifndef _SPINLOCKLIB_H_
#define _SPINLOCKLIB_H_

#define SPIN_LOCK_ISR_INIT(x, y)
#define SPIN_LOCK_ISR_TAKE(x)
#define SPIN_LOCK_ISR_GIVE(x)

#define SPIN_LOCK_TASK_TAKE(x)
#define SPIN_LOCK_TASK_GIVE(x)

typedef struct spinLockNdTimed_s {
	atomic32_t state;
	int options;
} spinLockNdTimed_t;

typedef struct spinlockIsr {
	UINT32 owner;
	int intKey;
	int flags;
} spinlockIsr_t;

typedef struct spinlockTask {
	int flags;
} spinlockTask_t;

void spinLockTaskInit(spinlockTask_t *lock, int flags);

#endif
