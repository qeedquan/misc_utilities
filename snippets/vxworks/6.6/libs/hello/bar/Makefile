#
#  Do not modify TGT_DIR
#
TGT_DIR=$(WIND_BASE)/target

#
#  DOC_FILES should be empty at this level, since drivers should
#  normally each be in a separate subdirectory of this level
#
DOC_FILES=

#
#  Change to reflect your own company name
#
LIB_BASE_NAMES	= hello

#
#  Normally, this is empty.  Object modules are built in the
#  driver-specific subdirectories.
#
OBJS_COMMON= bar.o 

SUBDIRS = zip

#
#  Do not modify OBJS
#
OBJS	= $(OBJS_COMMON) $(OBJS_$(CPU))

#
#  Do not modify the following include
#
include $(TGT_DIR)/h/make/rules.library
