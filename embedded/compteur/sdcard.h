#pragma once
#include <stdint.h>
#include "common/platform.h"

class sdcard {
public:
  sdcard();
  void info();
  void write(const char * filename, const uint8_t * data, const size_t length);
};
