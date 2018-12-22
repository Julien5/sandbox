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

  class TimeParser {
    StringAwaiter startAwaiter;
    char time[3]; // hour, minute, second
    nstring::STR<40> buffer;
    nstring::STR<8> retain;
    int parse_index;
    bool in_frame;
  public:
    TimeParser()
      : startAwaiter("Date")
      ,	time{-1}
      , buffer{}
      , retain{}
      , parse_index(-1)
      , in_frame(false)
    {}
    void read(const char * buffer);
    char* get() {
      debug(int(time[0]));
      debug(int(time[1]));
      debug(int(time[2]));
      if (time[0]>=0 && time[1]>=0 && time[2]>=0)
	return time;
      return NULL;
    }
  };

  int test();
}
