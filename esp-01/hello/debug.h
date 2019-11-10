#pragma once

#ifndef ARDUINO

#include <iostream>
#include <cassert>
#define DBG(X)						\
  do {									\
    std::cout << __FILE__ << ":" << __LINE__ << ":" << X << std::endl << std::flush; \
  } while(0)
#define TRACE()					\
  do {						\
    static int __counter__ = 0;			\
    std::cout << __FILE__;			\
    std::cout << ":";				\
    std::cout << __LINE__;			\
    std::cout << ":";				\
    std::cout << __counter__++ << "\n";		\
  } while(0)
#define F(X) (X)
#define PROGMEM 

#else

/*
 * note: in debug mode, DBGTX takes memory.
 * reduce NDATA
 */

#include "Arduino.h"
#include "HardwareSerial.h"
#if 1
#define DBGTX(X)				\
  do {						\
    Serial.print((X));				\
    Serial.flush();				\
  } while(0)
#define DBGTXLN(X)				\
  do {						\
    Serial.println((X));			\
    Serial.flush();				\
  } while(0)
#define TRACE()					\
  do {						\
    Serial.print(__FILE__);			\
    Serial.print(":");				\
    Serial.print(__LINE__);			\
    Serial.print(":");				\
    printMemory(0);				\
    Serial.flush();				\
  } while(0)
#else
#define DBGTX(X) ((void) 0)
#define DBGTXLN(X) ((void) 0)
#define TRACE() ((void) 0)
#endif
#define assert(ignore)
#define debug(X)((void) 0)
#define debug2(X,Y)((void) 0)
#endif
