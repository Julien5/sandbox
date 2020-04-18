#pragma once

#include <stdint.h>
#include "platform.h"

namespace wifi {
  class callback {
  public:
    virtual ~callback(){};
    virtual void status(uint8_t s) = 0;
    virtual void data_length(uint16_t total_length) = 0;
    virtual void data(uint8_t * data, size_t length) = 0;
    virtual void crc(bool ok) = 0;
  };
  
  class wifi {
  public:
    wifi();
    ~wifi();
    
    int get(const char* req, callback * r);
    int post(const char* req, const uint8_t * data, const uint16_t Ldata, callback * r);
  };
}
