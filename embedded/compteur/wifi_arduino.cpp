#include "wifi.h"

namespace wifi {
  wifi::wifi() {
  }

  wifi::~wifi() {
  }

  bool wifi::reset() {
    return false;
  }

  bool wifi::join() {
    return true;
  }

  bool wifi::get(const char* req, char** response) {
    return false;
  }
  
  int wifi::post(const char* req, const uint8_t * data, const int Ldata, char** response) {
    return 0;
  }
  
  bool wifi::enabled() const {
    return true;
  }
}
