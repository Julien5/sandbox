CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-I..
LDFLAGS=
LIBS=

SRCS := test.cpp ../parse.cpp
# Objs are all the sources, with .cpp replaced by .o
OBJS := $(SRCS:.cpp=.o)

%.o: %.c 
	$(CXX) $(CPPFLAGS) -c $<

all: test

test: $(OBJS)
	$(CXX) $(CPPFLAGS) -o test $(OBJS) $(LFLAGS) $(LIBS)

clean:
	$(RM) $(OBJS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^ -MF  ./.depend;

include .depend
