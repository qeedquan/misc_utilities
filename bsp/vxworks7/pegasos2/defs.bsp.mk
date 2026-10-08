DTC_FLAGS += -Wno-unique_unit_address

# Need to also define this flag in vsbConfig.h for the assembly code in the VSB to see the definition
CFLAGS += -DPPC_745x

ADDED_CFLAGS += -Wno-shorten-64-to-32 -Wno-implicit-int-conversion

BSP_MODULES = sysAUtil.o pci.o vt8231.o viaide.o i8254.o ds1385.o i6300wd.o viai2c.o nvram.o lpt.o m48t59.o util.o

BSPLIB = bsplib.a

$(BSPLIB): $(BSP_MODULES)
	 $(AR) rv $(BSPLIB) $(BSP_MODULES)

LIBS := $(BSPLIB) $(LIBS)
