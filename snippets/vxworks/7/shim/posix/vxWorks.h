#ifndef _VXWORKS_H_
#define _VXWORKS_H_

#define VX_DEBUG 0
#define VX_SYSCLK_RATE 60
#define VX_TICK_RATE 60

#if VX_DEBUG
#define eprintf(fmt, ...) fprintf(stderr, "%s:%d:%s " fmt, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#else
#define eprintf(fmt, ...)
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>
#include <spawn.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <poll.h>
#include <signal.h>
#include <limits.h>
#include <sys/statfs.h>
#include <sys/ioctl.h>
#include <malloc.h>

#include "types/vxTypes.h"
#include "vxWorksCommon.h"
#include "vxParams.h"
#include "vxAtomicLib.h"
#include "errnoLib.h"
#include "msgQLib.h"
#include "semLib.h"
#include "sysLib.h"
#include "eventLib.h"
#include "taskLib.h"
#include "clockLib.h"
#include "tickLib.h"
#include "objLib.h"
#include "sdLib.h"
#include "hostLib.h"
#include "memLib.h"
#include "ioLib.h"
#include "timerLib.h"
#include "mmuLib.h"
#include "spinLockLib.h"
#include "adrSpaceLib.h"

#define nelem(x) (sizeof(x) / sizeof(x[0]))
#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define clamp(x, a, b) min(max(x, a), b)

#define NELEMENTS nelem
#define ROUND_UP(x, a) (((x) + (a - 1)) & ~(a - 1))

#endif
