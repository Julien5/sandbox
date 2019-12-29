#include "platform.h"

#ifndef ARDUINO
#include <atomic>
std::atomic<test_ms> test_t(0);
test_ms millis() {
  return test_t.load(); 
}
void delay(test_ms d) {
  test_t+=d;
}
#endif
