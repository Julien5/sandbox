#pragma once

#include "common/rusttypes.h"

namespace crc {
    // init crc with 0x00
    void CRC8(u8 *init, const u8 *data, size_t length);
    int test();
}
