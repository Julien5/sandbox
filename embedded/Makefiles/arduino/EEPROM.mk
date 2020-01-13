include ../settings
include settings
include EEPROM
NAME:=$(XNAME)
SRCSDIR := /usr/share/arduino/libraries/$(NAME)
include build.mk
