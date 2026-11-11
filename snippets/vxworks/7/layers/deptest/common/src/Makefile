LIB_BASE_NAME = depcom
LIB_FORMAT = both

OBJS = depcom.o

ifeq ($(SPACE),user)
    include $(WIND_USR_MK)/rules.library.mk
else
    include $(WIND_KRNL_MK)/rules.library.mk
endif
