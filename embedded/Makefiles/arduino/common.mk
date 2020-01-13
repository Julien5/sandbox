include ../settings

include settings
include common
include LowPower
INCLUDE += -I/usr/share/arduino/libraries/EEPROM/

NAME:=common

all: deps lib

deps:
	make -C $(MAKEFILESDIR)/arduino -f LowPower lib

SRCSDIR := $(DIREMBEDDED)/$(NAME)
include build.mk

