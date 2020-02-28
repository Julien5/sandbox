#pragma once
#include "platform.h"

class serial {
public:
  void init();
  size_t write(uint8_t *buffer, size_t buffer_size);
  size_t read(uint8_t *buffer, size_t buffer_size, uint16_t timeout);
};
