#pragma once


#ifndef ARDUINO
#error "this source is only for arduino target"
#endif

#include "parse.h"
#include <stdint.h>
#include "wifi.h"

namespace wifi {
  class esp8266 : public interface {
    char enable_pin;
    bool m_enabled=false;
    bool m_joined=false;

    bool enable();
    void disable();
    void setTimeout(int t);
    bool ping();
    
  public:
    esp8266();
    ~esp8266();
    
    bool reset();
    bool join();   
    bool get(const char* req, char** response);
    int post(const char* req, const uint8_t * data, const int Ldata, char** response=0);
    bool enabled() const;
  };
}
