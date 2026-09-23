#ifndef _VXATOMICLIB_H_
#define _VXATOMICLIB_H_

#include <stdatomic.h>

#include "types/vxTypes.h"

typedef atomic_int_least32_t atomic32_t;
typedef atomic_int_least64_t atomic64_t;

#define vxAtomic32Add atomic_fetch_add
#define vxAtomic32Sub atomic_fetch_sub

#define vxAtomic32Get atomic_load
#define vxAtomic32Set atomic_store

#endif
