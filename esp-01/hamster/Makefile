CC=gcc
CXX=g++
RM=rm -f
LDFLAGS=
LIBS=-lpthread
COMMONDIR=../common
CXXFLAGS=-Wall -I. -I$(COMMONDIR) -std=c++14 -DDEBUG -g 

SRCS := \
$(COMMONDIR)/eeprom.cpp  \
$(COMMONDIR)/tickscounter.cpp \
$(COMMONDIR)/utils.cpp \
$(COMMONDIR)/platform.cpp\
$(COMMONDIR)/debug.cpp \
tests/test.cpp \
parse.cpp \
nstring.cpp \
freememory.cpp \
clock.cpp \
ui.cpp \
lcd.cpp \
wifi.cpp \
wifi_esp.cpp \
sensor.cpp \
application.cpp

# Objs are all the sources, with .cpp replaced by .o
OBJS := $(SRCS:.cpp=.o)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: test

test: $(OBJS)
	$(CXX) $(CXXFLAGS) -o test $(OBJS) $(LFLAGS) $(LIBS)

clean:
	$(RM) $(OBJS)

check:
	cppcheck --language=c++ --enable=all --template=gcc $(SRCS) main.ino

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CXXFLAGS) -MM $^ -MF  ./.depend;

# TODO fix makefiles
# include .depend
