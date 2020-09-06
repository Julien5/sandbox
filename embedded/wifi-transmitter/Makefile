NAME:=wifi-transmitter
MAKEFILESDIR := ../Makefiles
SRCSDIR := .

include $(MAKEFILESDIR)/settings
include $(MAKEFILESDIR)/pc/settings
include $(MAKEFILESDIR)/pc/common

DEFINES += -DNOMAIN

all: dir $(OBJS) libs lib

include $(MAKEFILESDIR)/pc/build.mk

libs:
	make -C $(MAKEFILESDIR)/pc -f common.mk lib

check:
	cppcheck --language=c++ --enable=all --template=gcc $(wildcard *.cpp)
