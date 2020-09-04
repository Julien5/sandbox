#pragma once

#if defined(DEVHOST)
// min and max are defined in Arduino.h
#include <vector>
#include <string>
#include "common/rusttypes.h"
namespace utils {
    std::vector<u8> hex_to_bytes(const std::string &hex);
    u8 *as_cbytes(std::vector<u8> &, int *L = 0);
};
#endif

namespace utils {
    void dump(const unsigned char *data_buffer, const unsigned int length);
}

long long fixed_atoll(char *s);
