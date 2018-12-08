CC=gcc
CXX=g++
RM=rm -f
CXXFLAGS=-Wall -I..
LDFLAGS=
LIBS=

debug: CXXFLAGS += -DDEBUG -g
debug: test

SRCS := test.cpp ../parse.cpp ../wifi.cpp ../nstring.cpp
# Objs are all the sources, with .cpp replaced by .o
OBJS := $(SRCS:.cpp=.o)

%.o: %.c 
	$(CXX) $(CXXFLAGS) -c $<

all: test

test: $(OBJS)
	$(CXX) $(CXXFLAGS) -o test $(OBJS) $(LFLAGS) $(LIBS)

clean:
	$(RM) $(OBJS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CXXFLAGS) -MM $^ -MF  ./.depend;

include .depend
