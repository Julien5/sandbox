#include "time.h"
#include "debug.h"

namespace time {
  uint64_t epoch_offset;
}

void time::set_current_epoch(uint64_t ms) {
  time::epoch_offset=ms;
}

uint64_t time::since_epoch() {
  return time::epoch_offset+since_reset();
}

#if defined(DEVHOST)
#include <atomic>
std::atomic<test_ms> test_t(0);
uint32_t time::since_reset() {
  return test_t.load(); 
}
void time::delay(test_ms d) {
  test_t+=d;
}
#endif

#if defined(ARDUINO)
#include "Arduino.h"
uint32_t time::since_reset() {
  return millis();
}

void time::delay(uint32_t d) {
  return ::delay(d);
}
#endif

#if defined(ESP8266)
extern "C" uint32_t esp_get_time(void);
uint32_t time::since_reset() {
  return esp_get_time()/1000;
}
#include "FreeRTOS.h"
#include "task.h"
void time::delay(uint32_t d) {
  vTaskDelay( d/portTICK_PERIOD_MS);
}
#endif
