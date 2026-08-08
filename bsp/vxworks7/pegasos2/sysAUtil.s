#define _ASMLANGUAGE
#include <vxWorks.h>
#include <sysLib.h>
#include <vsbConfig.h>
#include <asm.h>
#include <arch/ppc/private/adrSpaceArchLibP.h>
#include <prjParams.h>

#define TBLR_W 284
#define TBLR_R 268
#define TBUR_W 285
#define TBUR_R 269

FUNC_EXPORT(sysTimeBaseLGet)
FUNC_EXPORT(sysTimeBaseUGet)
FUNC_EXPORT(sysTimeBaseLPut)
FUNC_EXPORT(sysTimeBaseUPut)

    _WRS_TEXT_SEG_START

FUNC_BEGIN(sysTimeBaseLGet)
    mftb r3, TBLR_R
    bclr 20, 0
FUNC_END(sysTimeBaseLGet)

FUNC_BEGIN(sysTimeBaseUGet)
    mftb r3, TBUR_R
    bclr 20, 0
FUNC_END(sysTimeBaseUGet)

FUNC_BEGIN(sysTimeBaseLPut)
    sync
    mtspr   TBLR_W, r3
    isync
    bclr    20, 0
FUNC_END(sysTimeBaseLPut)

FUNC_BEGIN(sysTimeBaseUPut)
    sync
    mtspr   TBUR_W, r3
    isync
    bclr    20, 0
FUNC_END(sysTimeBaseUPut)
