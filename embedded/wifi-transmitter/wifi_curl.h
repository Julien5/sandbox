#pragma once
#include "common/wifi.h"

namespace wifi {
class wifi_curl {
public:
  wifi_curl();
  ~wifi_curl();
    
  int get(const char* req, callback * r);
  int post(const char* req, const uint8_t * data, const int Ldata, callback * r);
};
}
