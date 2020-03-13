#pragma once

#include <vector>
#include <stdint.h>
#include "platform.h"

struct message {
  uint8_t data[2048];
  size_t length={0};
  message();
};
