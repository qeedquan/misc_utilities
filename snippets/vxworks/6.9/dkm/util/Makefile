TGT_DIR = $(WIND_BASE)/target

# CPU = ARCH
TOOL = gnu
MYOUT = util.out
OBJS = bootdbg.o util.o

$(MYOUT): $(OBJS)
	$(LD_PARTIAL) -o $@ $(OBJS)

default: $(MYOUT)

include $(TGT_DIR)/h/make/rules.dkm.mk

CC_COMPILER += -g
CC_OPTIM =

