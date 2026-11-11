# This gets compiled as a library, but the .o files generated can be loaded as a dkm
# Example: ld < applog.o

TGT_DIR=$(WIND_BASE)/target

DOC_FILES=applog.c

LIB_BASE_NAME=applog

OBJS_COMMON = applog.o

OBJS = $(OBJS_COMMON) $(OBJS_$(CPU))

include $(TGT_DIR)/h/make/rules.library
CC_OPTIM=$(CC_OPTIM_DRIVER)
include  $(TGT_DIR)/h/make/defs.drivers

