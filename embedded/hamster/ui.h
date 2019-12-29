#pragma once
#include <stdint.h>
#include <stdlib.h>
namespace ui {
  void format(int32_t n, char * buffer, size_t L);
  void format_seconds(int32_t secs, char * buffer, size_t L);
  int test();
};
