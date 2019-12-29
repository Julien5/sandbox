ifndef SRCSDIR
SRCSDIR := /usr/share/arduino/libraries/$(NAME)
endif
OBJSDIR := /tmp/build/arduino/$(NAME)

# todo: remove duplicates.
INCLUDE += -I$(SRCSDIR) $(XINCLUDE)

SRCS := $(shell find $(SRCSDIR) -maxdepth 1 \( \( -name "*.cpp" -and -not -name "*_*" \) -or  -name "*_arduino.cpp" \))
OBJS := $(patsubst $(SRCSDIR)/%.cpp,$(OBJSDIR)/%.o,$(SRCS))

.PHONY: dir

dir:
	mkdir -p $(OBJSDIR) 

lib: dir $(OBJS) 
	$(AR) rcs $(OBJSDIR)/lib$(NAME).a $(OBJS)

exe: dir $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) $(XLIBS) $(LIBS) -o $(NAME).arduino

$(OBJS): $(OBJSDIR)/%.o : $(SRCSDIR)/%.cpp
	$(CXX) $(DEFINES) $(CXXFLAGS) -include Arduino.h $(XINCLUDE) $(INCLUDE) -c -o $@ $<

clean: dir
	find $(OBJSDIR) -type f -delete -print
