#pragma once

#include <stdint.h>

namespace Clock {
  using ms = uint32_t;
  using mn = uint16_t;
  void wake_up_after(ms m);
  ms since_start();
  void set_time(char h, char m, char s);
  void set_day(char d);
  ms millis_today();
  mn minutes_this_month();
  bool good();

  int test();
}
