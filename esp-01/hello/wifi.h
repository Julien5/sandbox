#pragma once

/*
  AT+CWLAP
  +CWLAP:(1,"JBO",-75,"00:1a:4f:00:41:92",2,107,0)
  +CWLAP:(3,"FRITZ!Box 7430 TJ",-78,"7c:ff:4d:c4:6b:f3",11,132,0)
*/


#include "nstring.h"
#include "parse.h"

namespace wifi {
  class esp8266 {
    int timeout;
  public:
    void setTimeout(int t) {
      timeout=t;
    }    
    bool reset();
    bool join();
    bool ping();
    nstring::STR<128> httpGET(const nstring::STR<128> &req);      
  };

  int test();

}
