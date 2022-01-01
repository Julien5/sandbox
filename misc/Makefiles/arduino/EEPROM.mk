include ../settings
include settings
include EEPROM
NAME:=$(XNAME)
SRCSDIR := $(ARDUINOCOREDIR)/libraries/$(NAME)/src/
include build.mk
