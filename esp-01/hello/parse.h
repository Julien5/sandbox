#pragma once

#include <string.h>
#include "nstring.h"

namespace parse {
  class StringAwaiter
  {
    const char * wanted; // 0-terminated
    int first_not_found;
  public:
    StringAwaiter(const char *w)
      :wanted(w)
      ,first_not_found(0)
    {};
    bool read(const char * buffer);
  };

  class AccessPoint {
  public:
    char name[16];
    int rssi;

    AccessPoint()
      : name{}
      , rssi(0)
    {}
  };
  
  class AccessPointParser {
    AccessPoint m_aps[8];
    int N;
    nstring::STR<128> buffer;
    nstring::STR<64> retain;
    int parse_index;
  public:
    AccessPointParser()
      : m_aps{}
      , N(0)
      , buffer{}
      , retain{}
      , parse_index(0)
    {}
    void read(const char * buffer);
    int size() const {
      return N;
    }
    AccessPoint get(int index) {
      return m_aps[index];
    }
  };

  int test();
}
