#include "common/time.h"
#include "common/debug.h"

using namespace common;

namespace common {
    namespace time {
        ms epoch_offset(0);
    }
}

common::time::ms::ms() : m_value(0) {}
common::time::ms::ms(u64 v) : m_value(v){};
common::time::ms::ms(const us &v) : m_value(v.value() / 1000){};
common::time::ms::operator us() const {
    return us(value() * 1000);
}
u64 common::time::ms::value() const {
    return m_value;
}
common::time::ms &common::time::ms::add(const ms &other) {
    m_value += other.value();
    return *this;
}

common::time::ms common::time::ms::since(const ms &older) const {
    return ms(value() - older.value());
}

bool common::time::ms::operator>(const ms &other) const {
    return value() > other.value();
}

common::time::us::us() : m_value(0) {}
common::time::us::us(u64 v) : m_value(v){};
common::time::us::us(const ms &v) : m_value(1000 * v.value()){};
common::time::us::operator ms() const {
    return ms(value() / 1000);
}
u64 common::time::us::value() const {
    return m_value;
}
common::time::us &common::time::us::add(const us &other) {
    m_value += other.value();
    return *this;
}
common::time::us common::time::us::since(const us &older) const {
    return us(value() - older.value());
}

void common::time::set_current_epoch(const ms &time) {
    common::time::epoch_offset = time;
}

common::time::ms common::time::since_epoch() {
    return ms(common::time::epoch_offset.value() + since_reset().value());
}

common::time::ms common::time::elapsed_since(const ms &t0) {
    return ms(common::time::since_reset().value() - t0.value());
}

#if defined(PC)
common::time::us test_t(common::time::us(0));
common::time::ms common::time::since_reset() {
    return test_t;
}
common::time::us common::time::since_reset_us() {
    return test_t;
}
void common::time::delay(const ms &delay) {
    test_t.add(delay);
}
void common::time::simulate(const us &delay) {
    test_t.add(delay);
}
void common::time::add_time_slept(const ms &delay) {
    assert(0);
}
#endif

#if defined(ARDUINO)
common::time::ms time_slept;
void common::time::add_time_slept(const ms &delay) {
    time_slept.add(delay);
}
#include "Arduino.h"
common::time::ms common::time::since_reset() {
    return ms(millis()).add(time_slept);
}
common::time::us common::time::since_reset_us() {
    return us(micros()).add(time_slept);
}
void common::time::delay(const ms &delay) {
    return ::delay(delay.value());
}
void common::time::simulate(const us &delay) {
    // nothing to do (there is no simulation on arduino)
}
#endif

#if defined(ESP8266)
extern "C" u32 esp_get_time(void);

common::time::ms common::time::since_reset() {
    return ms(esp_get_time() / 1000);
}
common::time::us common::time::since_reset_us() {
    return us(esp_get_time());
}
#include "FreeRTOS.h"
#include "task.h"
void common::time::delay(const ms &delay) {
    vTaskDelay(delay.value() / portTICK_PERIOD_MS);
}
void common::time::add_time_slept(const ms &delay) {
    assert(0);
}
#endif
