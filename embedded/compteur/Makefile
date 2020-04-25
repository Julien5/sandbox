NAME:=compteur
MAKEFILESDIR := ../Makefiles
SRCSDIR := .

include $(MAKEFILESDIR)/settings
include $(MAKEFILESDIR)/devhost/settings
include $(MAKEFILESDIR)/devhost/wifi-transmitter
include $(MAKEFILESDIR)/devhost/common

all: dir $(OBJS) libs exe

include $(MAKEFILESDIR)/devhost/build.mk

libs:
	make -C $(MAKEFILESDIR)/devhost -f common.mk lib
	make -C $(MAKEFILESDIR)/devhost -f wifi-transmitter.mk lib

check:
	cppcheck --language=c++ --enable=all --template=gcc $(wildcard *.cpp)

info:
	$(info INCLUDE is $(INCLUDE))
	$(info OBJS is $(OBJS))
	$(info CXXOBJS is $(CXXOBJS))
	$(info COBJS is $(COBJS))

