#include "Arduino.h"

namespace analog {
  uint16_t read() {
    return analogRead(0);
  }
};
