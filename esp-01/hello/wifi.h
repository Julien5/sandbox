#pragma once

#include "nstring.h"
#include "parse.h"

#include <stdint.h>

namespace wifi {
  class esp8266 {
    int timeout;
    char enable_pin;
    parse::TimeParser time_parser;
    bool m_enabled=false;
  public:
    esp8266(char pin);
    ~esp8266();
    
    bool reset();
    bool enable();
    void disable();
    
    void setTimeout(int t);
 
    bool join();
    bool ping();
    bool get(const char* req);
    int post(const char* req, const uint8_t * data, const int Ldata);

    bool enabled() const;
  };

  int test();

}
