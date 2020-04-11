#pragma once

#include "common/platform.h"

namespace crc
{
  // init crc with 0x00
  void CRC8(uint8_t *init, const uint8_t* data, size_t length);
  int test();
}
