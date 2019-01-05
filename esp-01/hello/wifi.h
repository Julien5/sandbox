#pragma once

#include "nstring.h"
#include "parse.h"

#include <stdint.h>

namespace wifi {
  class esp8266 {
    int timeout;
    char enable_pin;
    parse::TimeParser time_parser;
  public:
    esp8266(char pin);
    ~esp8266();
    
    bool reset();
    void enable();
    void disable();
    
    void setTimeout(int t);
 
    bool join();
    bool ping();
    bool get_time(char *h, char *m, char *s);
    bool get(const char* req);
    int post(const char* req, const uint8_t * data, const int Ldata);
  };

  int test();

}
