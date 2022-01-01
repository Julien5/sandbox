#pragma once
#include <stdint.h>
#include <stdlib.h>
namespace ui {
  void format(i32 n, char * buffer, size_t L);
  void format_seconds(i32 secs, char * buffer, size_t L);
  int test();
};
