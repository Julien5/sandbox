#pragma once

#include "nstring.h"

namespace wifi {
  class esp8266 {
    int timeout;
    char enable_pin;
  public:
    esp8266(char pin);
    ~esp8266();
    
    bool reset();
    void enable();
    void disable();
    
    void setTimeout(int t);
 
    bool join();
    bool ping();
    bool get(const char* req);
    int post(const char* req, const char * data, const int Ldata);
  };

  int test();

}
