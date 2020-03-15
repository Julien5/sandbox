#pragma once

#include <stdint.h>

namespace Clock {
  using ms = uint32_t;
  using mn = uint16_t;
  ms millis_since_start();
  mn minutes_since_start();
  int test();
}
