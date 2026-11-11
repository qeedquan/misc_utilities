LIB_BASE_NAME = hello
LIB_FORMAT = both

OBJS = hello.o goodbye.o greet.o

ifeq ($(SPACE),user)
    include $(WIND_USR_MK)/rules.library.mk
else
    include $(WIND_KRNL_MK)/rules.library.mk
endif
