NAME:=compteur
MAKEFILESDIR := ../Makefiles
SRCSDIR := .

include $(MAKEFILESDIR)/settings
include $(MAKEFILESDIR)/pc/settings
include $(MAKEFILESDIR)/pc/wifi-transmitter
include $(MAKEFILESDIR)/pc/common

all: dir $(OBJS) libs exe

include $(MAKEFILESDIR)/pc/build.mk

libs:
	make -C $(MAKEFILESDIR)/pc -f common.mk lib
	make -C $(MAKEFILESDIR)/pc -f wifi-transmitter.mk lib

check:
	cppcheck --language=c++ --enable=all --template=gcc $(wildcard *.cpp)

info:
	$(info INCLUDE is $(INCLUDE))
	$(info OBJS is $(OBJS))
	$(info CXXOBJS is $(CXXOBJS))
	$(info COBJS is $(COBJS))

