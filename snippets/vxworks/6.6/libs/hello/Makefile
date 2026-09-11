# Example of having two different directories building the same code that will be linked into the same library
# There is another folder that will build into a different library than the one above 
# The libraries will be built into target/lib/ in $WIND_BASE

# Building:
# > make CPU=PCPENTIUM4 TOOL=diab DUAL_BUILD=false COMMON_DIR=false
# > make CPU=PCPENTIUM4 TOOL=gnu DUAL_BUILD=false COMMON_DIR=false

TGT_DIR=$(WIND_BASE)/target

CLEANLIBS = NO

COMMON_SUBDIRS  = $(subst Makefile,,$(wildcard *))

include $(TGT_DIR)/h/make/defs.default
include $(TGT_DIR)/h/tool/$(TOOL_FAMILY)/make.$(CPU)$(TOOL)

__SUBDIRS = $(COMMON_SUBDIRS)

SUBDIRS = $(strip $(foreach dir, $(__SUBDIRS),$(wildcard $(dir))))

# overrides DOCSUBDIRS = $(SUBDIRS) in rules-lib.$(WIND_HOST_TYPE) to
# make sure all necessary doc directories are built.

DOCSUBDIRS = $(COMMON_SUBDIRS)

include $(TGT_DIR)/h/make/rules.library
include $(TGT_DIR)/h/make/rules.vxbus

# If doing a doc build, ensure arch/Makefile is built.
# The rules are in $(TGT_DIR)/h/make/rules-lib.unix

man:    $(TGT_DIR)/src/arch/Makefile

