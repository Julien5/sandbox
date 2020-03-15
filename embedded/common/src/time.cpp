#include "common/time.h"
#include "common/debug.h"

namespace Time {
  uint64_t epoch_offset;
}

void Time::set_current_epoch(uint64_t ms) {
  Time::epoch_offset=ms;
}

uint64_t Time::since_epoch() {
  return Time::epoch_offset+since_reset();
}

#if defined(DEVHOST)
#include <atomic>
typedef uint32_t test_ms;
std::atomic<test_ms> test_t(0);
uint32_t Time::since_reset() {
  return test_t.load(); 
}
void Time::delay(test_ms d) {
  test_t+=d;
}
#endif

#if defined(ARDUINO)
#include "Arduino.h"
uint32_t Time::since_reset() {
  return millis();
}

void Time::delay(uint32_t d) {
  return ::delay(d);
}
#endif

#if defined(ESP8266)
extern "C" uint32_t esp_get_time(void);
uint32_t Time::since_reset() {
  return esp_get_time()/1000;
}
#include "FreeRTOS.h"
#include "task.h"
void Time::delay(uint32_t d) {
  vTaskDelay( d/portTICK_PERIOD_MS);
}
#endif
