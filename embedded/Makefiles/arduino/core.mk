include ../settings
include settings
include core
NAME:=$(XNAME)
SRCSDIR := $(ARDUINOCOREDIR)
OBJSDIR := /tmp/build/arduino/$(NAME)

CXXSRCS := $(wildcard $(SRCSDIR)/*.cpp)
CSRCS := $(wildcard $(SRCSDIR)/*.c)
CSRCS += $(wildcard $(SRCSDIR)/avr-libc/*.c)

CXXOBJS := $(patsubst $(SRCSDIR)/%.cpp,$(OBJSDIR)/%.o,$(CXXSRCS))
COBJS += $(patsubst $(SRCSDIR)/%.c,$(OBJSDIR)/%.o,$(CSRCS))

.PHONY: dir

dir:
	mkdir -p $(OBJSDIR) $(OBJSDIR)/avr-libc

lib: dir $(COBJS) $(CXXOBJS)
	$(AR) rcs $(OBJSDIR)/lib$(NAME).a $(COBJS) $(CXXOBJS) 

$(CXXOBJS): $(OBJSDIR)/%.o : $(SRCSDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDE) -c -o $@ $<

$(COBJS): $(OBJSDIR)/%.o : $(SRCSDIR)/%.c
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDE) -std=gnu11 -c -o $@ $<

clean:
	find $(OBJSDIR) -type f -delete -print

