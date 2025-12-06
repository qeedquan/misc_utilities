LOAD_ADDR = $(LOCAL_MEM_PHYS_ADRS)

vxWorks.bin: vxWorks
	- @ $(RM) $@
	$(EXTRACT_BIN) vxWorks $@

BSP_MODULES = pl031rtc.o pl061gpio.o

BSPLIB = bsplib.a

$(BSPLIB): $(BSP_MODULES)
	$(AR) rv $(BSPLIB) $(BSP_MODULES)

LIBS := $(BSPLIB) $(LIBS)
