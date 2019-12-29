ifndef SRCSDIR
$(error SRCSDIR not set)
endif
OBJSDIR := /tmp/build/esp8266/$(NAME)

# todo: remove duplicates.
INCLUDE += -I$(SRCSDIR) $(XINCLUDE)

SRCS := $(shell find $(SRCSDIR) -maxdepth 1 \( \( -name "*.cpp" -and -not -name "*_*" \) -or  -name "*_esp8266.cpp" \))
OBJS := $(patsubst $(SRCSDIR)/%.cpp,$(OBJSDIR)/%.o,$(SRCS))

.PHONY: dir

dir:
	mkdir -p $(OBJSDIR) 

lib: dir $(OBJS) 
	$(AR) rcs $(OBJSDIR)/lib$(NAME).a $(OBJS)

exe: dir $(OBJS)
	$(CXX) $(OBJS) $(XLIBS) $(LIBS) $(LDFLAGS) -o $(NAME).esp8266

$(OBJS): $(OBJSDIR)/%.o : $(SRCSDIR)/%.cpp
	$(CXX) $(DEFINES) $(CXXFLAGS) $(XINCLUDE) $(INCLUDE) -c -o $@ $<

clean: dir
	find $(OBJSDIR) -type f -delete -print
