#pragma once

#ifndef ARDUINO
#include <iostream>
#include <cassert>
#define debug(X)							\
  do									\
    {									\
      std::cout << __LINE__ << ":" << #X << "=" << (X) << std::endl;	\
    } while(0)
#define DBGTX(X)					\
  do							\
    {							\
      std::cout << __LINE__ << ":" << #X << "=" << (X);	\
    } while(0)
#define DBGTXLN(X)							\
  do									\
    {									\
      std::cout << __LINE__ << ":" << #X << "=" << (X) << std::endl;	\
    } while(0)

using test_ms = uint32_t;
test_ms t=0;
test_ms millis() {
  return test_t; 
}
void delay(test_ms d) {
  test_t+=_delay;
}

#else
#include "Arduino.h"
#include "HardwareSerial.h"
#define DBGTX(X)				\
  do						\
    {						\
      Serial.write((X));			\
    } while(0)
#define DBGTXLN(X)				\
  do						\
    {						\
      Serial.println((X));			\
    } while(0)
//#define DBGTX(X) ((void) 0)
//#define DBGTXLN(X) ((void) 0)
#define assert(ignore)
#define debug(X)((void) 0)
#endif
