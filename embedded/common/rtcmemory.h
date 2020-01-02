#pragma once
#include <stdint.h>
class rtcmemory {
public:
  rtcmemory();
  bool read(void *des_addr, uint16_t save_size);
  bool write(void *src_addr, uint16_t save_size);
};
