NAME:=wifi-transmitter
MAKEFILESDIR := ../Makefiles
SRCSDIR := .

include $(MAKEFILESDIR)/settings
include $(MAKEFILESDIR)/devhost/settings
include $(MAKEFILESDIR)/devhost/common

DEFINES += -DNOMAIN

all: dir $(OBJS) libs lib

include $(MAKEFILESDIR)/devhost/build.mk

libs:
	make -C $(MAKEFILESDIR)/devhost -f common.mk lib

check:
	cppcheck --language=c++ --enable=all --template=gcc $(wildcard *.cpp)
