#include "platform.h"

#ifndef ARDUINO
void digitalWrite(uint8_t, uint8_t) {
}

void attachInterrupt(uint8_t interrupt, void ISR(void), uint8_t mode) {
}
#endif

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
