#pragma once

#include <stdint.h>
#include "platform.h"

namespace wifi {
  class callback {
  public:
    virtual ~callback(){};
    virtual void operator()(uint8_t * data, size_t length) = 0;
  };
  
  class wifi {
  public:
    wifi();
    ~wifi();
    
    int get(const char* req, callback * r);
    int post(const char* req, const uint8_t * data, const int Ldata, callback * r);
  };
}
