#include "common/time.h"
#include "common/debug.h"

using namespace common;
namespace common {
    namespace Time {
        u64 epoch_offset;
    }
}

void common::Time::set_current_epoch(u64 ms) {
    common::Time::epoch_offset = ms;
}

u64 common::Time::since_epoch() {
    return common::Time::epoch_offset + since_reset();
}

#if defined(PC)
#include <atomic>
typedef u32 test_ms;
std::atomic<test_ms> test_t(0);
u32 common::Time::since_reset() {
    return test_t.load();
}
void common::Time::delay(test_ms d) {
    test_t += d;
}
#endif

#if defined(ARDUINO)
#include "Arduino.h"
u32 common::Time::since_reset() {
    return millis();
}

void common::Time::delay(u32 d) {
    return ::delay(d);
}
#endif

#if defined(ESP8266)
extern "C" u32
esp_get_time(void);
u32 common::Time::since_reset() {
    return esp_get_time() / 1000;
}
#include "FreeRTOS.h"
#include "task.h"
void common::Time::delay(u32 d) {
    vTaskDelay(d / portTICK_PERIOD_MS);
}
#endif
