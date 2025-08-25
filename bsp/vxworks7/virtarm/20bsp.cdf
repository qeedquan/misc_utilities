Selection BOARD_SELECTION {
    NAME        Board Selection
    COUNT       1-1
    CHILDREN    INCLUDE_VIRT_ARM
    DEFAULTS    INCLUDE_VIRT_ARM
    _CHILDREN   FOLDER_HIDDEN
}

Component INCLUDE_VIRT_ARM {
    NAME        QEMU Virt ARM
    SYNOPSIS    This component provides support for the \
                QEMU Virt ARM.
}

Parameter DTS_FILE {
    NAME        DTS file name to be used
    DEFAULT     virtarm.dts
}

Parameter LOCAL_MEM_PHYS_ADRS {
    NAME        local memory physical base address
    SYNOPSIS    Base physical address.
    DEFAULT     0x40000000
}

Parameter STATIC_MMU_TABLE_BASE {
    DEFAULT    (LOCAL_MEM_PHYS_ADRS + 0x80000)
}

Parameter IMA_SIZE {
    NAME        IMA(Initial Mapped Area) size
    SYNOPSIS    IMA(Initial Mapped Area) will be mapped to MMU for \
                the early initialization phase before usrMmuInit(), \
                so the size should be large enough to hold the entire VxWorks Kernel image.
    DEFAULT     0x10000000
}

Parameter RAM_LOW_ADRS {
    NAME        Runtime kernel entry address
    DEFAULT     0x00100000
}

Parameter RAM_HIGH_ADRS {
    NAME        Runtime kernel high address
    DEFAULT     0x01000000
}

Parameter KERNEL_LOAD_ADRS {
    NAME        Runtime kernel load address
    DEFAULT     0x40100000
}

Parameter LOCAL_MEM_LOCAL_ADRS {
    NAME        system memory start address
    DEFAULT     0
}

Parameter VEC_BASE_ADRS {
    NAME        Vector base address
    SYNOPSIS    Vector base address
    DEFAULT     ((char *) (LOCAL_MEM_LOCAL_ADRS) + 0x200000)
}

Parameter DTB_RELOC_ADDR {
    NAME        DTB relocateds address
    SYNOPSIS    DTB(Device Tree Blob) need be relocated to one safe address
                to avoid be overwritten, so it should be below RAM_LOW_ADRS
                and the reserved start region, and enough for the entire
                DTB.
    TYPE        void *
    DEFAULT     (LOCAL_MEM_LOCAL_ADRS + 0x10000)
}

Parameter DTB_MAX_LEN {
    NAME        DTB maximum length
    SYNOPSIS    DTB(Device Tree Blob) need be relocated to one safe address
                to avoid be overwritten, so it should be below RAM_LOW_ADRS
                and the reserved start region, and enough for the entire
                DTB.
    TYPE        int
    DEFAULT     0x100000
}

Parameter ISR_STACK_SIZE {
    NAME        ISR stack size
    SYNOPSIS    ISR Stack size (bytes)
    DEFAULT     0x2000
}

Parameter DEFAULT_BOOT_LINE {
    NAME        default boot line
    SYNOPSIS    Default boot line string
    TYPE        string
    DEFAULT     "fs(0,0)host:/vxWorks e=10.0.2.15 h=10.0.2.2 g=10.0.2.2 u=target pw=vxTarget o=gei"
}

Parameter CONSOLE_BAUD_RATE {
    NAME        baud rate of console port
    DEFAULT     115200
}

Parameter SYSCLK_TIMER_NAME {
    NAME            System Clock Device Name (NULL is auto-assign)
    TYPE            string
    DEFAULT         NULL
}

Parameter SYSCLK_TIMER_NUM {
    NAME            System Clock Timer Number
    TYPE            int
    DEFAULT         1
}

Parameter AUXCLK_TIMER_NAME {
    NAME            Auxiliary Clock Device Name (NULL is auto-assign)
    TYPE            string
    DEFAULT         NULL
}

Parameter AUXCLK_TIMER_NUM {
    NAME            Auxiliary Clock Timer Number
    TYPE            int
    DEFAULT         0
}

Profile BSP_DEFAULT {
    PROFILES    PROFILE_OS_DEFAULT
    COMPONENTS  += INCLUDE_SIO
}

Profile PROFILE_DEVELOPMENT {
    COMPONENTS      -= INCLUDE_NET_SYM_TBL
    COMPONENTS      += INCLUDE_STANDALONE_SYM_TBL
}

