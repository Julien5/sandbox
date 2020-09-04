#pragma once
#include "common/wifi.h"

namespace wifi {
class wifi_curl {
public:
  wifi_curl();
  ~wifi_curl();
    
  int get(const char* req, callback * r);
  int post(const char* req, const u8 * data, const u16 Ldata, callback * r);
};
}
