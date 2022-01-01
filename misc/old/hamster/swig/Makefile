NAME:=hamster-swig
MAKEFILESDIR := ../../Makefiles
SRCSDIR := .

all: dir $(WRAPPERSRCS) $(OBJS) libs shared

include $(MAKEFILESDIR)/settings
include $(MAKEFILESDIR)/pc/settings
include $(MAKEFILESDIR)/pc/common

WRAPPERI:=$(SRCSDIR)/statistics.i
WRAPPERSRCS:=$(SRCSDIR)/statistics_wrap_pc.cpp

SRCS += $(WRAPPERSRCS)

PYTHON_VERSION := $(shell python3 -c "import sys;t='{v[0]}.{v[1]}'.format(v=list(sys.version_info[:2]));sys.stdout.write(t)")
INCLUDE += -I/usr/include/python$(PYTHON_VERSION)m/

include $(MAKEFILESDIR)/pc/build.mk

$(WRAPPERSRCS): $(WRAPPERI)
	swig3.0 -c++ -python $(INCLUDE) -o $@ $< 

print:
	$(info SRCSDIR is $(SRCSDIR))	
	$(info OBJSDIR is $(OBJSDIR))
	$(info INCLUDE is $(INCLUDE))
	$(info OBJS is $(OBJS))
	$(info SRCS is $(SRCS))

libs:
	make -C $(MAKEFILESDIR)/pc -f common lib

deploy:
	cp hamster.py  $(shell python3 -m site --user-site)
	cp libhamster-swig.so $(shell python3 -m site --user-site)/_hamster.so

