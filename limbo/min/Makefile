include Makefile.core

CXXSRCS := \
BareMinimum.cpp

# Objs are all the sources, with .cpp replaced by .o
OBJS += $(notdir $(CXXSRCS:.cpp=.o))

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(OBJS)
	$(AR) rcs libcore.a $(OBJS) 
	$(CXX) $(CXXFLAGS) libcore.a $(LFLAGS) $(LIBS) -o min.elf

clean:
	$(RM) *.o

