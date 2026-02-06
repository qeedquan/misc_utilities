#define _ASMLANGUAGE
#include <vxWorks.h>
#include <vsbConfig.h>
#include <asm.h>
#include <regs.h>
#include <arch/arm/arm.h>
#include "prjParams.h"
 
FUNC_EXPORT(sysInit)

FUNC_IMPORT(usrInit)

#ifdef INCLUDE_STANDALONE_DTB
        .data

#define TO_STRING(exp)                  #exp
#define DTS_ASM_FILE(file)              TO_STRING (file)

#include DTS_ASM_FILE (DTB_ASM)

#endif /* INCLUDE_STANDALONE_DTB */

#if defined (__GNUC__)
        .section .text.entry, "ax"
#else
#error "TOOL not supported!"
#endif

        .code   32

/*

This routine is the system start-up entry point for VxWorks in RAM, the first code executed after booting.

QEMU setups everything for us so we do not need to initialize anything.

On startup, QEMU has two ways of passing us information:
1. For guests using the Linux kernel boot protocol (this means any non-ELF file passed to the QEMU -kernel option)
the address of the DTB is passed in a register (r2 for 32-bit guests, or x0 for 64-bit guests)

2. For guests booting as "bare-metal" (any other kind of boot), the DTB is at the start of RAM (0x4000_0000)

Use option 2 because we want the debugging information stored in the ELF image for GDB.

Option 2 presents a challenge for VxWorks.
By default the FDT parsing code on VxWorks cannot load any DTB file that is placed at the 0 (NULL) address.
When VxWorks enables the MMU it will map the start of the RAM address at address 0, so it see the DTB address as NULL and fail to load it.

One workaround is to just extract the DTB and hardcode it into the image.
This will fail if QEMU changes the DTB in later versions.
QEMU does allow the user to specify version when using virt so the version can be fixed, this allows us to sidestep the DTB issue.

*/

FUNC_LABEL (sysInit)
        // set initial stack pointer so stack grows down from start of code
        ADR     sp, sysInit
        MOV     fp, #0

        LDR     r1, =sysInit
        ADR     r6, sysInit
        SUB     r6, r6, r1

        // store the DTB address into a variable
#ifdef INCLUDE_STANDALONE_DTB
        LDR     r0, =dt_blob_start
#endif
        LDR     r1, =gpDtbInit
        ADD     r1, r1, r6
        STR     r0, [r1]
        MOV     r0, #0

        LDR     r1, =usrInit
        ADD     r1, r1, r6
        // legacy BOOT_COLD
        MOV     r0, #2
        BX      r1
FUNC_END(sysInit)
