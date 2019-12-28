ifndef SRCSDIR
$(error SRCSDIR undefined)
endif
OBJSDIR := /tmp/build/devhost/$(NAME)

# todo: remove duplicates.
INCLUDE += -I$(SRCSDIR) $(XINCLUDE)

SRCS := $(wildcard $(SRCSDIR)/*.cpp)
OBJS := $(patsubst $(SRCSDIR)/%.cpp,$(OBJSDIR)/%.o,$(SRCS))

.PHONY: dir

dir:
	mkdir -p $(OBJSDIR) 

lib: dir $(OBJS) 
	$(AR) rcs $(OBJSDIR)/lib$(NAME).a $(OBJS)

exe: dir $(OBJS)
	$(CXX) $(LDFLAGS) $(OBJS) $(XLIBS) $(LIBS) -o $(NAME).devhost

$(OBJS): $(OBJSDIR)/%.o : $(SRCSDIR)/%.cpp
	$(CXX) $(DEFINES) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

clean: dir
	find $(OBJSDIR) -type f -delete -print
