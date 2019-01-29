#pragma once

#include <stdint.h>

namespace Clock {
  using ms = uint32_t;
  using mn = uint16_t;
  void wake_up_after(ms m);
  ms millis_since_start();
  mn minutes_since_start();
  int test();
}
