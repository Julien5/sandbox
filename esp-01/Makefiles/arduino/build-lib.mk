ifndef SRCSDIR
SRCSDIR := /usr/share/arduino/libraries/$(NAME)
endif
OBJSDIR := /tmp/build/arduino/$(NAME)

# todo: remove duplicates.
INCLUDE += -I$(SRCSDIR) $(XINCLUDE)

SRCS := $(wildcard $(SRCSDIR)/*.cpp)
OBJS := $(patsubst $(SRCSDIR)/%.cpp,$(OBJSDIR)/%.o,$(SRCS))

.PHONY: dir

dir:
	mkdir -p $(OBJSDIR) 

all: dir $(OBJS) 
	$(AR) rcs $(OBJSDIR)/lib$(NAME).a $(OBJS) 

$(OBJS): $(OBJSDIR)/%.o : $(SRCSDIR)/%.cpp
	$(CXX) $(DEFINES) $(CXXFLAGS) -include Arduino.h $(INCLUDE) -c -o $@ $<

clean: 
	find $(OBJSDIR) -type f -delete -print
