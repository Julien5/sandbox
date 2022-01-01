ifndef SRCSDIR
$(error SRCSDIR undefined)
endif
OBJSDIR := /tmp/build/pc/$(NAME)

# todo: remove duplicates.
INCLUDE += -I$(SRCSDIR) $(XINCLUDE)

ifndef SRCS
SRCS := $(shell find $(SRCSDIR) -maxdepth 1 \( \( -name "*.cpp" -and -not -name "*_*" \) -or  -name "*_pc.cpp" \))
endif

OBJS := $(patsubst $(SRCSDIR)/%.cpp,$(OBJSDIR)/%.o,$(SRCS))

.PHONY: dir lib exe shared

dir:
	mkdir -p $(OBJSDIR) 

lib: dir $(OBJS) 
	$(AR) rcs $(OBJSDIR)/lib$(NAME).a $(OBJS)

exe: dir $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) $(XLIBS) $(LIBS) -o $(NAME).pc

shared: dir $(OBJS)
	$(CXX) -shared $(LDFLAGS) $(OBJS) $(XLIBS) $(LIBS) -o lib$(NAME).so

$(OBJS): $(OBJSDIR)/%.o : $(SRCSDIR)/%.cpp
	$(CXX) $(DEFINES) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

clean: dir
	find $(OBJSDIR) -type f -delete -print

print:
	$(info OBJS is $(OBJS))
	$(info SRCSDIR is $(SRCSDIR))
	$(info OBJSDIR is $(OBJSDIR))
