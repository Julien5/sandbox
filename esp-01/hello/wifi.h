#pragma once

#include "parse.h"

#include <stdint.h>

namespace wifi {
  class esp8266 {
    int timeout;
    char enable_pin;
    bool m_enabled=false;
    bool m_joined=false;
  public:
    esp8266(char pin);
    ~esp8266();
    
    bool reset();
    bool enable();
    void disable();
    
    void setTimeout(int t);
 
    bool join();
    bool ping();
    bool get(const char* req, char** response);
    int post(const char* req, const uint8_t * data, const int Ldata, char** response);

    bool enabled() const;
  };

  int test();

}
