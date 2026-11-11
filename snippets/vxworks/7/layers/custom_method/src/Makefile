include $(WIND_KRNL_MK)/defs.vsbvars.mk

LIB_BASE_NAME = custom_method
LIB_FORMAT = both

include $(WIND_KRNL_MK)/defs.library.mk

SRC_C = $(wildcard *.c)
METHODM = $(sort $(wildcard *.m))

OBJS = $(METHODM:.m=.o) $(SRC_C:.c=.o)

BUILD_ALL_OBJS = TRUE

include $(WIND_KRNL_MK)/rules.library.mk
include $(WIND_KRNL_MK)/defs.drivers.mk
