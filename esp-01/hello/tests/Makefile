CC=gcc
CXX=g++
RM=rm -f
CXXFLAGS=-Wall -I.. -std=c++14
LDFLAGS=
LIBS=

debug: CXXFLAGS += -DDEBUG -g
debug: test

SRCS := \
test.cpp \
../parse.cpp \
../nstring.cpp \
../statistics.cpp \
../eeprom.cpp  \
../freememory.cpp \
../debug.cpp \
../clock.cpp \
../tickscounter.cpp

# ../wifi.cpp 
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
