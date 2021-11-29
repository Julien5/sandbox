#include "common/time.h"
#include "common/debug.h"

using namespace common;
namespace common {
    namespace time {
        u64 epoch_offset;
    }
}

void common::time::set_current_epoch(u64 ms) {
    common::time::epoch_offset = ms;
}

u64 common::time::since_epoch() {
    return common::time::epoch_offset + since_reset();
}

u32 common::time::elapsed_since(const u32 t0) {
    return common::time::since_reset() - t0;
}

#if defined(PC)
#include <atomic>
typedef u32 test_ms;
std::atomic<test_ms> test_t(0);
u32 common::time::since_reset() {
    return test_t.load();
}
u64 common::time::micros_since_reset() {
    return 1000 * test_t.load();
}
void common::time::delay(test_ms d) {
    test_t += d;
}
#endif

#if defined(ARDUINO)
#include "Arduino.h"
u32 common::time::since_reset() {
    return millis();
}
u64 common::time::micros_since_reset() {
    return micros();
}
void common::time::delay(u32 d) {
    return ::delay(d);
}
#endif

#if defined(ESP8266)
extern "C" u32 esp_get_time(void);

u32 common::time::since_reset() {
    return esp_get_time() / 1000;
}
u64 common::time::micros_since_reset() {
    return esp_get_time();
}
#include "FreeRTOS.h"
#include "task.h"
void common::time::delay(u32 d) {
    vTaskDelay(d / portTICK_PERIOD_MS);
}
#endif
