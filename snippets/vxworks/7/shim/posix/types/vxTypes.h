#ifndef _VXTYPES_H_
#define _VXTYPES_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <threads.h>
#include "vxmain.h"

typedef unsigned char u_char;
typedef unsigned char uchar_t;
typedef unsigned short ushort_t;
typedef unsigned long ulong_t;

typedef int BOOL;

typedef int STATUS;

typedef int (*FUNCPTR)(...);
typedef void (*VOIDFUNCPTR)(...);

typedef unsigned int MMU_ATTR;

typedef signed char CHAR;
typedef signed short SHORT;
typedef signed int INT;
typedef signed long LONG;

typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef unsigned long long ULLONG;

typedef void VOID;

typedef int8_t INT8;
typedef int16_t INT16;
typedef int32_t INT32;
typedef int64_t INT64;

typedef unsigned int UINT;
typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint64_t UINT64;

typedef INT32 _Vx_ticks_t;
typedef INT64 _Vx_ticks64_t;

typedef int _Vx_freq_t;
typedef int _Vx_exit_code_t;

typedef INT64 _Vx_usr_arg_t;

typedef INT64 _Vx_ioctl_arg_t;

typedef UINT64 VIRT_ADDR;
typedef UINT64 PHYS_ADDR;

typedef int atomic_t;

#define FALSE 0
#define TRUE 1

#ifndef __FUNCTION__
#define __FUNCTION__ __func__
#endif

#ifndef IMPORT
#define IMPORT extern
#endif

#ifndef LOCAL
#define LOCAL static
#endif

#ifndef FAST
#define FAST register
#endif

#endif
