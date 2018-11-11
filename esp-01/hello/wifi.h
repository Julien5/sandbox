#pragma once

/*
  AT+CWLAP
  +CWLAP:(1,"JBO",-75,"00:1a:4f:00:41:92",2,107,0)
  +CWLAP:(3,"FRITZ!Box 7430 TJ",-78,"7c:ff:4d:c4:6b:f3",11,132,0)
*/

namespace wifi {
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
    AccessPoint m_aps[6];
    int N;
    char buffer[128];
    char retain[16];
    int parse_index;
  public:
    AccessPointParser()
      : m_aps{}
      , N(0)
      , buffer{}
      , retain{}
      , parse_index(0)
    {}
    void read(char * buffer);
    int size() const {
      return N+1;
    }
    AccessPoint get(int index) {
      return m_aps[index];
    }
  };

  int test();

}
