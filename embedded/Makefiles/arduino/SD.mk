include ../settings
include settings
include SD
NAME:=$(XNAME)
SRCSDIR := /usr/share/arduino/libraries/$(NAME)
include build.mk
