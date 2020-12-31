include ../settings
include settings
include LiquidCrystal
NAME:=$(XNAME)
SRCSDIR := $(DIR3RDPARTY)/$(NAME)
include build.mk
