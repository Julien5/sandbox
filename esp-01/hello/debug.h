#pragma once

#ifndef ARDUINO
#include <iostream>
#include <cassert>
#define debug(X)							\
  do									\
    {									\
      std::cout << __LINE__ << ":" << #X << "=" << (X) << std::endl;	\
    } while(0)
#else
#include "HardwareSerial.h"
#define DDBGTX(X)							\
  do									\
    {									\
      Serial.write((X));						\
    } while(0)
#define DDBGTXLN(X)							\
  do									\
    {									\
      Serial.println((X));						\
    } while(0)
#define DBGTX(X) ((void) 0)
#define DBGTXLN(X) ((void) 0)
#define assert(ignore)
#define debug(X)((void) 0)
#endif
