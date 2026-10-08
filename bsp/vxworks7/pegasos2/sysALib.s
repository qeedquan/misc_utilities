#define _ASMLANGUAGE
#include <vxWorks.h>
#include <sysLib.h>
#include <vsbConfig.h>
#include <asm.h>
#include <arch/ppc/private/adrSpaceArchLibP.h>
#include <prjParams.h>

FUNC_IMPORT(vxCpuInit)
FUNC_IMPORT(usrInit)

FUNC_EXPORT(sysInit)
FUNC_EXPORT(_sysInit)

    .data
#ifdef _WRS_CONFIG_STANDALONE_DTB
#define TO_STRING(exp)          #exp
#define DTS_ASM_FILE(file)      TO_STRING (file)

#include DTS_ASM_FILE (DTB_ASM)
#endif /* _WRS_CONFIG_STANDALONE_DTB */

#if ((RAM_LOW_ADRS - LOCAL_MEM_LOCAL_ADRS) != KERNEL_LOAD_ADRS)
#error Please modify RAM_LOW_ADRS or KERNEL_LOAD_ADRS to ensure that \
"(RAM_LOW_ADRS - LOCAL_MEM_LOCAL_ADRS) = KERNEL_LOAD_ADRS"
#endif

    _WRS_TEXT_SEG_START

DATA_IMPORT (gpDtbInit)

    .section .text.entry, "ax", @progbits

/*

Entry point for the system, the bootloader will execute this code on startup.
It initializes the system, configures the MMU for the initial loading process,
saves the DTB address, and jumps to C usrInit for the rest of the system init.

The PPC architecture has a few different standardization of booting, one is specified by the the ePAPR document (Chapter 5.4 of ePAPR v1.0)
which describes the following setup:

MSR:
  PR=0 supervisor state
  EE=0 interrupts disabled
  ME=0 machine check interrupt disabled
  IS=0,DS=0 address space 0

r3: Effective address of the device tree image. Note: This address shall be 8 bytes aligned in memory.
r6: 0x45504150, ePAPR magic value used to distinguish from non-ePAPR-compliant firmware
r7: The size of IMA(Initial Mapped Area) in bytes
r4, r5, r8, r9: 0

TCR: WRC=0, no watchdog timer reset will occur

NOTE:
  1. <pOf> is unused because bootng from Open Firmware is unsupported now;
  2. The initial stack is placed in .data section, and mapped with the entire image vxWorks.

If Embedded FDT mode is enabled, the legacy boot mode is used and the input arguments are ignored, and IMA size is from CDF.

VOID sysInit(
     void  *pDtb,      // Effective address of the device tree image
     UINT32 reserved1, // shall be 0
     UINT32 reserved2, // shall be 0
     UINT32 magic,     // magic number, should be 0x45504150
     size_t imaSize    // shall be the size of IMA
)

If the boot process is loaded by OpenFirmware, the alternative setup is used:
(The QEMU target uses this setup)

r1: Stack provided by OpenFirmware
r3: Reserved for Platform binding (usually initrd)
r4: Reserved for Platform binding (usually initrd_size)
r5: OpenFirmware client entry point
r6: Arguments
r7: Arguments length

*/

FUNC_LABEL(_sysInit)
FUNC_BEGIN(sysInit)
    li      r29, BOOT_CLEAR
    bl      cold

    li      r29, BOOT_NORMAL
cold:
#ifdef _WRS_CONFIG_STANDALONE_DTB
    /* save IMA size */

    lis     r31, HI(IMA_SIZE)
    ori     r31, r31, LO(IMA_SIZE)
#else /* !_WRS_CONFIG_STANDALONE_DTB */

    /* save DTB address and IMA size */

    mr      r30, r3
    mr      r31, r7
#endif /* _WRS_CONFIG_STANDALONE_DTB */

#ifdef INCLUDE_WARM_BOOT
    cmpwi   r29, BOOT_CLEAR
    bne     skipBackup

    bl      vxSdaInit
    /* get frame stack */

    lis     sp, HIADJ(RAM_LOW_ADRS)
    addi    sp, sp, LO(RAM_LOW_ADRS)
    addi    sp, sp, -FRAMEBASESZ

    li      r3, 0
    bl      warmBootBackup
skipBackup:
#endif /* INCLUDE_WARM_BOOT */

    /* initialize CPU */

    bl  vxCpuInit

    /* obtain runtime address */

    bl  rtAddr
rtAddr:
    mflr    r3      /* runtime address */

    /* calculate offset from link address */

    lis r4, HI(LOCAL_MEM_LOCAL_ADRS)
    ori r4, r4, LO(LOCAL_MEM_LOCAL_ADRS)
    lis r5, HI(rtAddr)
    ori r5, r5, LO(rtAddr)
    sub r5, r5, r4

    /* calculate runtime address base */

    sub     r3, r3, r5

    /* adjust DTB address */

#ifndef _WRS_CONFIG_STANDALONE_DTB
    sub r30, r30, r3    /* get offset */
    add r30, r30, r4
#endif /* !_WRS_CONFIG_STANDALONE_DTB */

    /* initialize MMU early */

    mr  r5, r31
    bl  vxMmuEarlyInit

    /* save DTB address */

#ifdef _WRS_CONFIG_STANDALONE_DTB
    lis r30, HI(dt_blob_start)
    ori r30, r30, LO(dt_blob_start)
#endif /* _WRS_CONFIG_STANDALONE_DTB */

    lis r6, HIADJ(gpDtbInit)
    stw r30, LO(gpDtbInit)(r6)

    /* get frame stack */

    lis     sp, HIADJ(RAM_LOW_ADRS)
    addi    sp, sp, LO(RAM_LOW_ADRS)
    addi    sp, sp, -FRAMEBASESZ

    /* jump to C routine */

    mr      r3, r29   /* restore the boot type */
    b       usrInit
FUNC_END(sysInit)

