#pragma once

#include <stdint.h>

namespace time {
  // set with an external time source.
  void set_current_epoch(uint64_t ms);
  uint64_t since_epoch();
  uint32_t since_reset();
  void delay(uint32_t d);
};
