NAME:=hamster-swig
MAKEFILESDIR := ../../Makefiles
SRCSDIR := .

include $(MAKEFILESDIR)/settings
include $(MAKEFILESDIR)/devhost/settings
include $(MAKEFILESDIR)/devhost/common
include $(MAKEFILESDIR)/devhost/build.mk

WRAPPERI:=$(SRCSDIR)/statistics.i
WRAPPERSRCS:=$(SRCSDIR)/statistics_wrap_devhost.cpp
WRAPPEROBJS:=$(patsubst $(SRCSDIR)/%.cpp,$(OBJSDIR)/%.o,$(WRAPPERSRCS))

SRCS += $(WRAPPERSRCS)
OBJS += $(WRAPPEROBJS)

.PHONY: wrapper

$(WRAPPERSRCS): $(WRAPPERI)
	$(SWIG) $(SWIGFLAGS) -o $@ $< 

all: dir $(WRAPPERSRCS) $(OBJS) libs shared 


print:
	$(info SRCSDIR is $(SRCSDIR))	
	$(info OBJSDIR is $(OBJSDIR))
	$(info INCLUDE is $(INCLUDE))
	$(info OBJS is $(OBJS))
	$(info SRCS is $(SRCS))


SWIG = swig3.0
SWIGFLAGS = -c++ -python $(INCLUDE)
PYTHON_VERSION := $(shell python3 -c "import sys;t='{v[0]}.{v[1]}'.format(v=list(sys.version_info[:2]));sys.stdout.write(t)")
INCLUDE += -I/usr/include/python$(PYTHON_VERSION)m/

statistics_wrap.cpp: statistics.i
	$(SWIG) $(SWIGFLAGS) -o $@ $< 

libs:
	make -C $(MAKEFILESDIR)/devhost -f common lib

deploy:
	cp hamster.py  $(shell python3 -m site --user-site)
	cp libhamster-swig.so $(shell python3 -m site --user-site)/_hamster.so

$(OBJS): $(OBJSDIR)/%.o : $(SRCSDIR)/%.cpp
	$(CXX) $(DEFINES) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<
