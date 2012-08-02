MODULE := engines/sprint

MODULE_OBJS := \
	database.o \
	detection.o \
	graphics.o \
	script.o \
	sprint.o

# This module can be built as a plugin
ifeq ($(ENABLE_SPRINT), DYNAMIC_PLUGIN)
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
