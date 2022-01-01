include ../settings
include settings
include common
include wifi-transmitter

NAME:=wifi-transmitter

DEFINES += -DNOMAIN

all: lib

SRCSDIR := $(DIREMBEDDED)/$(NAME)

include build.mk
