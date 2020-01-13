include ../settings
include settings
include LiquidCrystal
NAME:=$(XNAME)
SRCSDIR := /usr/share/arduino/libraries/$(NAME)
include build.mk
