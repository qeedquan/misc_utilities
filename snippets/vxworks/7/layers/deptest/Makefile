include $(WIND_KRNL_MK)/defs.layers.mk

ifdef _WRS_CONFIG_DEPTEST_COMMON
    KERNEL_PUBLIC_H_DIRS += common/h
    BUILD_DIRS += common/src
endif

ifdef _WRS_CONFIG_DEPTEST_A
    KERNEL_PUBLIC_H_DIRS += depa/h
    BUILD_DIRS += depa/src
endif

ifdef _WRS_CONFIG_DEPTEST_B
    KERNEL_PUBLIC_H_DIRS += depb/h
    BUILD_DIRS += depb/src
endif

include $(WIND_KRNL_MK)/rules.layers.mk
