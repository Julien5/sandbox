#include "platform.h"

#if defined(DEVHOST)
#include <atomic>
std::atomic<test_ms> test_t(0);
test_ms millis() {
  return test_t.load(); 
}
void delay(test_ms d) {
  test_t+=d;
}
#endif

#if defined(ESP8266)
#include "espressif/esp_common.h"
test_ms millis() {
  return sdk_system_get_time()/1000;
}

#include "FreeRTOS.h"
#include "task.h"

void delay(test_ms d) {
  vTaskDelay( d/portTICK_PERIOD_MS);
}
#endif
