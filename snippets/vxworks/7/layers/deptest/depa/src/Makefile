LIB_BASE_NAME = depa
LIB_FORMAT = both

OBJS = depa.o deps.o

ifeq ($(SPACE),user)
    include $(WIND_USR_MK)/rules.library.mk
else
    include $(WIND_KRNL_MK)/rules.library.mk
endif
