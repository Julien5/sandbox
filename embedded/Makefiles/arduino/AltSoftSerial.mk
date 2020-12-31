include ../settings
include settings
include AltSoftSerial

NAME:=$(XNAME)
SRCSDIR := /opt/arduino/libraries/$(NAME)
include build.mk

