#include "common/time.h"
#include "common/debug.h"

namespace Time {
    u64 epoch_offset;
}

void Time::set_current_epoch(u64 ms) {
    Time::epoch_offset = ms;
}

u64 Time::since_epoch() {
    return Time::epoch_offset + since_reset();
}

#if defined(DEVHOST)
#include <atomic>
typedef u32 test_ms;
std::atomic<test_ms> test_t(0);
u32 Time::since_reset() {
    return test_t.load();
}
void Time::delay(test_ms d) {
    test_t += d;
}
#endif

#if defined(ARDUINO)
#include "Arduino.h"
u32 Time::since_reset() {
    return millis();
}

void Time::delay(u32 d) {
    return ::delay(d);
}
#endif

#if defined(ESP8266)
extern "C" u32
esp_get_time(void);
u32 Time::since_reset() {
    return esp_get_time() / 1000;
}
#include "FreeRTOS.h"
#include "task.h"
void Time::delay(u32 d) {
    vTaskDelay(d / portTICK_PERIOD_MS);
}
#endif
