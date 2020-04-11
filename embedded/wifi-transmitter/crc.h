#pragma once
#include "common/platform.h"
namespace crc
{
  uint8_t CRC8(const uint8_t* data, size_t length);
  int test();
}
