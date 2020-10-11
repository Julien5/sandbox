include ../Makefiles/settings

include $(MAKEFILESDIR)/pc/settings
include $(MAKEFILESDIR)/pc/common

all: dir $(OBJS) deps exe

NAME := test
SRCSDIR := .

include $(MAKEFILESDIR)/pc/build.mk

deps:
	make -C $(MAKEFILESDIR)/pc -f common.mk lib

info:
	$(info MAKEFILESDIR is $(MAKEFILESDIR))


