include ../settings
include settings
include SD
NAME:=$(XNAME)
SRCSDIR := /usr/share/arduino/libraries/$(NAME)
INCLUDE += -I$(SRCSDIR) -I$(SRCSDIR)/utility
SRCS := $(shell find $(SRCSDIR) -name "*.cpp")
include build.mk

print:
	$(info MAKEFILESDIR is $(MAKEFILESDIR))	
	$(info INCLUDE is $(INCLUDE))
	$(info XINCLUDE is $(XINCLUDE))
	$(info OBJS is $(OBJS))
	$(info CXXOBJS is $(CXXOBJS))
	$(info COBJS is $(COBJS))
