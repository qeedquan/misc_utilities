Bsp virtarm {
    NAME                board support package
    CPU                 ARMARCH7 CORTEX_A15
    ENDIAN              little
    BUILD_SPECS         default
    BUILD_TARGETS       uVxWorks vxWorks vxWorks.bin
    FP                  hard soft
    MP_OPTIONS          UP SMP
    DATA_MODEL          ILP32
    REQUIRES            INCLUDE_KERNEL                  \
                        INCLUDE_CPU_INIT                \
                        INCLUDE_MEM_DESC                \
                        INCLUDE_BOARD_LIB_INIT          \
                        DRV_BUS_FDT_SIMPLE              \
                        INCLUDE_EARLY_MMU               \
                        INCLUDE_DEVCLK_SYS              \
                        INCLUDE_TIMER_SYS               \
                        INCLUDE_PCI_AUTOCONF            \
                        DRV_INTCTLR_FDT_ARM_GIC_V3      \
                        DRV_INTCTLR_FDT_ARM_GIC_V3_ITS  \
                        INCLUDE_GEI825XX_VXB_END        \
                        DRV_PCI_ECAM                    \
                        DRV_ARM_GEN_TIMER               \
                        DRV_CLK_FIXED_RATE              \
                        DRV_SIO_FDT_ARM_AMBA_PL011      \
                        INCLUDE_STANDALONE_DTB          \
                        BOARD_SELECTION
}
