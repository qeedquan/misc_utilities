TGT_DIR = $(WIND_BASE)/target

# CPU = ARCH
TOOL = gnu
MYOUT = hello.out
OBJS = hello.o

$(MYOUT): $(OBJS)
	$(LD_PARTIAL) -o $@ $(OBJS)

default: $(MYOUT)

include $(TGT_DIR)/h/make/rules.dkm.mk

CC_COMPILER += -g
CC_OPTIM =

