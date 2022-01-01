include ../settings
include settings
include common

NAME:=common

all: lib

SRCSDIR := $(DIREMBEDDED)/$(NAME)/src

include build.mk

