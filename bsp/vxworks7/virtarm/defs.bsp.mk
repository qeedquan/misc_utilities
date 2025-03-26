LOAD_ADDR = $(LOCAL_MEM_PHYS_ADRS)

vxWorks.bin: vxWorks
	- @ $(RM) $@
	$(EXTRACT_BIN) vxWorks $@
