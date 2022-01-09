#pragma once

#include "common/rusttypes.h"
#include "common/time.h"

namespace common {
    class serial {
        u8 rx_crc8;
        u8 tx_crc8;

      public:
        serial();
        usize write(u8 *buffer, usize buffer_size);
        usize read(u8 *buffer, usize buffer_size, u16 timeout);
        bool begin();
        bool end();
        void reset();

        // loops until L bytes are read.
        // timeout is passed to read() in the loop.
        bool read_until(u8 *addr, const usize &L, const u16 &timeout);
        bool wait_for_begin(const common::time::ms &timeout);
        bool check_end();
    };
}
