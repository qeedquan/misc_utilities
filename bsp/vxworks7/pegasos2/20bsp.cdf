Bsp pegasos2 {
    REQUIRES      INCLUDE_KERNEL            \
                  INCLUDE_CPU_INIT          \
                  INCLUDE_MEM_DESC          \
                  DRV_BUS_FDT_SIMPLE        \
                  INCLUDE_TIMER_SYS         \
                  INCLUDE_SIO               \
                  INCLUDE_CACHE_SUPPORT     \
                  INCLUDE_PCI_AUTOCONF      \
                  INCLUDE_ISR_DEFER         \
                  INCLUDE_WARM_BOOT         \
                  INCLUDE_WARM_BOOT_PROTECT \
                  INCLUDE_WATCHDOGDRV       \
                  DRV_INT_FDT_MV64360       \
                  INCLUDE_EEPROMDRV         \
                  DRV_I2C_EEPROM            \
                  INCLUDE_GEI825XX_VXB_END  \
                  BOARD_SELECTION
}

Selection BOARD_SELECTION {
    NAME        Board Selection
    COUNT       1-1
    CHILDREN    INCLUDE_PEGASOS2
    DEFAULTS    INCLUDE_PEGASOS2
    _CHILDREN   FOLDER_HIDDEN
}

Component INCLUDE_PEGASOS2 {
    NAME        PEGASOS2
    SYNOPSIS    This component is used to select the configuration of \
                PEGASOS2.
}

Parameter CONSOLE_BAUD_RATE {
    DEFAULT     115200
}

Parameter RAM_LOW_ADRS {
    NAME        Runtime kernel entry address
    DEFAULT     0x00100000
}

Parameter KERNEL_LOAD_ADRS {
    NAME        Runtime kernel load address
    DEFAULT     0x00100000
}

Parameter SYS_CLK_RATE_MIN {
    NAME        Minimum system clock rate
    SYNOPSIS    This parameter specifies the sets the minimum system clock rate. \
                Its default value is 3. Attempting to set a value below the \
                minimum returns an error.
    DEFAULT     (19)
}

Parameter SYS_CLK_RATE_MAX {
    NAME        Maximum system clock rate
    SYNOPSIS    This parameter specifies the sets the maximum system clock rate. \
                Its default value is 19. Attempting to set a value above the \
                maximum returns an error.
    DEFAULT     (5000)
}

Parameter SYSCLK_TIMER_NAME {
    NAME        System Clock Device Name (NULL is auto-assign)
    TYPE        string
    DEFAULT     "i8254-timer"
}

Parameter SYSCLK_TIMER_NUM {
    NAME        System Clock Timer Number
    TYPE        int
    DEFAULT     0
}

Parameter TIMESTAMP_TIMER_NAME {
    NAME        Timestamp Device Name (NULL is auto-assign)
    TYPE        string
    DEFAULT     "decTimer"
}

Component DRV_TIMER_PPC_DEC {
    INCLUDE_WHEN    INCLUDE_TIMESTAMP
}

Component INCLUDE_L2_CACHE {
    NAME            L2 cache support
    SYNOPSIS        This component configures your project to support the L2 cache.
    _CHILDREN       FOLDER_MEMORY
    REQUIRES        INCLUDE_CACHE_SUPPORT \
                    DRV_L2_CACHE_PPC750
    INCLUDE_WHEN    INCLUDE_CACHE_SUPPORT
}

Parameter DEFAULT_BOOT_LINE {
    NAME        Default boot line
    SYNOPSIS    This parameter provides the default boot line string.
    TYPE        string
    DEFAULT     "fs(0,0)host:/vxWorks e=10.0.2.15 h=10.0.2.2 g=10.0.2.2 u=target pw=vxTarget o=gei"
}

#ifdef _WRS_CONFIG_STANDALONE_DTB
Bsp pegasos2 {
    REQUIRES += SELECT_STANDALONE_DTB_SRC
}

/*

DTB could be built into VxWorks kernel image if _WRS_CONFIG_STANDALONE_DTB option is defined.
For this mode: IMA_SIZE need be set to one value that is large enough for entire VxWorks image.

*/  
Selection SELECT_STANDALONE_DTB_SRC {
    NAME        select standalone DTB source
    CHILDREN    INCLUDE_PEGASOS2_DTB
    COUNT       1-1
    DEFAULTS    INCLUDE_PEGASOS2_DTB
    _CHILDREN   FOLDER_BSP_CONFIG
    }

Component INCLUDE_PEGASOS2_DTB {
    NAME        Pegasos 2 DTB source
    CFG_PARAMS  IMA_SIZE
}

Parameter DTS_FILE {  
    NAME        DTS file for DTB
    DEFAULT     (INCLUDE_PEGASOS2_DTB)::(pegasos2.dts)
}     

Parameter IMA_SIZE {
    NAME        IMA(Initial Mapped Area) size
    SYNOPSIS    IMA(Initial Mapped Area) will be mapped to MMU for \
                the early initialization phase before usrMmuInit(), \
                so the size should be large enough to hold the entire VxWorks Kernel image.
    DEFAULT     0x10000000
}
#endif /* _WRS_CONFIG_STANDALONE_DTB */

Profile BSP_DEFAULT {
    NAME        Default BSP profile
    SYNOPSIS    Profile to include default BSP components and parameters.
    COMPONENTS  INCLUDE_DEBUG_KPRINTF INCLUDE_END
    PROFILES    PROFILE_OS_DEFAULT \
                PROFILE_ARCH_DEFAULT
}

#ifdef _WRS_CONFIG_FP_ksoft
Bsp pegasos2 {
    REQUIRES += INCLUDE_HW_FP
}
#endif

Profile PROFILE_DEVELOPMENT {
	COMPONENTS	-= INCLUDE_NET_SYM_TBL
	COMPONENTS	+= INCLUDE_STANDALONE_SYM_TBL
}
