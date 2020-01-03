#pragma once
#include <stdint.h>

#pragma pack(push,4)
struct transient {
  static bool load(transient *t);
  static bool save(transient *);
  static constexpr uint8_t MAGIC=0xAB;
  uint8_t magic=MAGIC;
  uint32_t n_wakeup=0;
  float a_fast=0;
  float a_slow=0;
};
#pragma pack(pop)

