#pragma once

#include <stdint.h>

namespace clock {
  using ms = uint32_t;
  void wake_up_after(ms m);
  ms since_start();
  void set_time(char h, char m, char s);
  ms millis_today();
}
