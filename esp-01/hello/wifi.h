#pragma once

#include "nstring.h"

namespace wifi {
  class esp8266 {
    int timeout;
  public:
    esp8266();
    void setTimeout(int t);
    bool reset();
    bool join();
    bool ping();
    bool get(const char* req);
    bool post(const char* req, const char * data, const int Ldata);
  };

  int test();

}
