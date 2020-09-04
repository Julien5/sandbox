#pragma once

#include "common/rusttypes.h"

class serial {
    u8 rx_crc8;
    u8 tx_crc8;
    i16 read(u8 *buffer, size_t buffer_size, u16 timeout);

  public:
    serial();
    // used by emitter
    size_t write(u8 *buffer, size_t buffer_size);
    bool begin();
    bool end();

    // used by receiver
    bool read_until(u8 *addr, const size_t &L, const u16 &timeout = 0);
    bool wait_for_begin();
    bool check_end();
};
