MODULE := engines/jmp

MODULE_OBJS = \
	bit.o \
	detection.o \
	graphics.o \
	jman.o \
	jmp.o \
	journey.o \
	sound.o
	


# This module can be built as a plugin
ifdef BUILD_PLUGINS
PLUGIN := 1
endif

# Include common rules
include $(srcdir)/rules.mk
