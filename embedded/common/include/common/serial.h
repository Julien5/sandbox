#pragma once
#include "common/platform.h"

class serial {
public:
  serial();
  size_t write(uint8_t *buffer, size_t buffer_size);
  size_t read(uint8_t *buffer, size_t buffer_size, uint16_t timeout);
  bool read_until(uint8_t * addr, const size_t &L);
  bool begin();
  bool wait_for_begin();
};
