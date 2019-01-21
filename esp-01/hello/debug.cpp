#include "debug.h"

#ifndef ARDUINO
test_ms test_t=0;
test_ms millis() {
  return test_t; 
}
void delay(test_ms d) {
  test_t+=d;
}
#endif
