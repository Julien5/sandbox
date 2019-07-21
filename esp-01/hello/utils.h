#pragma once

#ifndef ARDUINO

// min and max are defined in Arduino.h
#include <vector>
#include <string>
#include <stdint.h>

#define max(a,b)				\
  ({ __typeof__ (a) _a = (a);			\
    __typeof__ (b) _b = (b);			\
    _a > _b ? _a : _b; })


#define min(a,b)				\
  ({ __typeof__ (a) _a = (a);			\
    __typeof__ (b) _b = (b);			\
    _a < _b ? _a : _b; })

namespace utils {
  std::vector<uint8_t> hex_to_bytes(const std::string &hex);
  uint8_t * as_cbytes(std::vector<uint8_t> &, int *L=0);
};

#endif

long long fixed_atoll(char *s);
