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
#define assert(ignore)
#define debug(X)((void) 0)
#endif
