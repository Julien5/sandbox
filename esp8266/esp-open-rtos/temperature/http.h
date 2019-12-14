#pragma once
#include <stdint.h>
namespace http {
  typedef void (*get_callback)(uint8_t *data, const int16_t length);
  void get(get_callback callback);
  void post(uint8_t *data, const uint16_t length, get_callback callback);
}
