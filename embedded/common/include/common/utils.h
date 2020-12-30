#pragma once

#include "common/rusttypes.h"

#if defined(PC)
// min and max are defined in Arduino.h
#include <vector>
#include <string>
namespace utils {
    std::vector<u8> hex_to_bytes(const std::string &hex);
    u8 *as_cbytes(std::vector<u8> &, int *L = 0);
};
#endif

namespace utils {
    void dump(const u8 *data_buffer, const size_t length);
}

long long fixed_atoll(char *s);
