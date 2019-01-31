#pragma once

#ifndef ARDUINO

#include <vector>
#include <string>
#include <stdint.h>

namespace utils {
  std::vector<uint8_t> hex_to_bytes(const std::string &hex);
  uint8_t * as_cbytes(std::vector<uint8_t> &, int *L=0);
};

#endif
