#pragma once

#include "parse.h"

#include <stdint.h>

namespace wifi {
  class interface {
  public:    
    virtual bool reset() = 0;
    virtual bool join() = 0;
    virtual bool get(const char* req, char** response) = 0;
    virtual int post(const char* req, const uint8_t * data, const int Ldata, char** response=0) = 0;
    virtual bool enabled() const = 0;
    int test_upload();
  };
  
  class mock : public interface {
  public:
    bool reset();
    bool join();   
    bool get(const char* req, char** response);
    int post(const char* req, const uint8_t * data, const int Ldata, char** response=0);
    bool enabled() const;
  };
}
