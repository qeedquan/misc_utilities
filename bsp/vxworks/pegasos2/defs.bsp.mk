DTC_FLAGS += -Wno-unique_unit_address
ADDED_CFLAGS += -Wno-shorten-64-to-32 -Wno-implicit-int-conversion

BSP_MODULES = pci.o vt8231.o viaide.o i8254.o ds1385.o viai2c.o lpt.o util.o

BSPLIB = bsplib.a

$(BSPLIB): $(BSP_MODULES)
	 $(AR) rv $(BSPLIB) $(BSP_MODULES)

LIBS := $(BSPLIB) $(LIBS)
