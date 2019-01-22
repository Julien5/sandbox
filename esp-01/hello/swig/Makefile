# swig3.0 -c++ -python -o statistics_wrap.cpp statistics.i
# swig3.0 -c++ -python  statistics.i -o statistics_wrap.cpp
#
# g++ -fpic -c
# ../eeprom.cpp
# ../clock.cpp
# ../statistics.cpp
# ../debug.cpp
# statistics_wrap.cpp -I/usr/include/python3.6m/
#
# g++ -shared clock.o eeprom.o debug.o statistics.o statistics_wrap.o -o _statistics.so
#
# python3 test.py

CXX = g++
SWIG = swig3.0
SWIGFLAGS = -c++ -python 
CXXFLAGS = -c -fpic -Wall -I/usr/include/python3.6m/ 
LDFLAGS = -shared

SRCS := ../statistics.cpp ../eeprom.cpp ../debug.cpp ../clock.cpp statistics_wrap.cpp
# SRCS := x.cpp 

OBJS := $(SRCS:.cpp=.o)

_statistics.so: $(OBJS)
	$(CXX)  -o _statistics.so $(LDFLAGS) $^

%.o: ../%.cpp
	$(CXX) $(CXXFLAGS) $? -o $@

statistics_wrap.cpp: statistics.i
	$(SWIG) $(SWIGFLAGS) -o statistics_wrap.cpp $< 
