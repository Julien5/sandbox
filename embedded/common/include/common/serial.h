#pragma once
#include "common/platform.h"

class serial {
  uint8_t rx_crc8;
  uint8_t tx_crc8;
  size_t read(uint8_t *buffer, size_t buffer_size, uint16_t timeout);
  
public:
  serial();
  // used by emitter
  size_t write(uint8_t *buffer, size_t buffer_size);
  bool begin();
  bool end();

  // used by receiver
  bool read_until(uint8_t * addr, const size_t &L);
  bool wait_for_begin();
  bool check_end();
};
