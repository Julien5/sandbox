include ../settings
include settings
include common

NAME:=common

all: deps lib

deps:
	make -C $(MAKEFILESDIR)/arduino -f LowPower.mk lib
	make -C $(MAKEFILESDIR)/arduino -f EEPROM.mk lib
	make -C $(MAKEFILESDIR)/arduino -f AltSoftSerial.mk lib
	make -C $(MAKEFILESDIR)/arduino -f LiquidCrystal.mk lib	

SRCSDIR := $(DIREMBEDDED)/$(NAME)/src

include build.mk

