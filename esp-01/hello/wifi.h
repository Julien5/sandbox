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
    nstring::STR<128> httpGET(const nstring::STR<128> &req);      
  };

  int test();

}
