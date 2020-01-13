include ../settings
include settings
include common

NAME:=common

all: deps lib

deps:
	make -C $(MAKEFILESDIR)/arduino -f LowPower.mk lib

INCLUDE += -I/usr/share/arduino/libraries/EEPROM/
SRCSDIR := $(DIREMBEDDED)/$(NAME)

include build.mk

