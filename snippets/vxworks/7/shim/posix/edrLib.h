#ifndef _EDRLIB_H_
#define _EDRLIB_H_

#include "types/vxTypes.h"

#define S_edrLib_NOT_INITIALIZED (M_edrLib | 1)
#define S_edrLib_ERRLOG_CORRUPTED (M_edrLib | 2)
#define S_edrLib_ERRLOG_INCOMPATIBLE (M_edrLib | 3)
#define S_edrLib_PROTECTION_FAILURE (M_edrLib | 4)
#define S_edrLib_PMREGION_ERROR (M_edrLib | 5)
#define S_edrLib_INJECT_HOOK_TABLE_FULL (M_edrLib | 7)
#define S_edrLib_INJECT_HOOK_NOT_FOUND (M_edrLib | 8)
#define S_edrLib_TEXT_HOOK_TABLE_FULL (M_edrLib | 9)
#define S_edrLib_TEXT_HOOK_NOT_FOUND (M_edrLib | 10)
#define S_edrLib_PP_HOOK_TABLE_FULL (M_edrLib | 11)
#define S_edrLib_PP_HOOK_NOT_FOUND (M_edrLib | 12)
#define S_edrLib_INVALID_OPTION (M_edrLib | 6)

#define EDR_USER_WARNING_INJECT(trace, msg)
#define EDR_USER_INFO_INJECT(trace, msg)
#define EDR_USER_FATAL_INJECT(trace, msg)
#define EDR_RTP_FATAL_INJECT(trace, msg)

int edrFlagsGet(void);
BOOL edrIsDebugMode(void);

#endif
