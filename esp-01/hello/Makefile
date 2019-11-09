CC=gcc
CXX=g++
RM=rm -f
CXXFLAGS=-Wall -I. -std=c++14 -DDEBUG -g
LDFLAGS=
LIBS=-lpthread

SRCS := \
tests/test.cpp \
parse.cpp \
nstring.cpp \
statistics.cpp \
eeprom.cpp  \
freememory.cpp \
debug.cpp \
clock.cpp \
tickscounter.cpp \
utils.cpp \
ui.cpp \
lcd.cpp \
wifi.cpp \
wifi_esp.cpp \
sensor.cpp \
platform.cpp 

# ../wifi.cpp 
# Objs are all the sources, with .cpp replaced by .o
OBJS := $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

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
