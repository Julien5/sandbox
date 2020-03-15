#pragma once

#if defined(DEVHOST)
// min and max are defined in Arduino.h
#include <vector>
#include <string>
#include <stdint.h>
namespace utils {
  std::vector<uint8_t> hex_to_bytes(const std::string &hex);
  uint8_t * as_cbytes(std::vector<uint8_t> &, int *L=0);
};
#endif

namespace utils {
  void dump(const unsigned char *data_buffer, const unsigned int length);
}

long long fixed_atoll(char *s);

