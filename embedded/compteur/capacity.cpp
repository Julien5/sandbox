#include "capacity.h"
#include "common/analog.h"
#include "common/time.h"

const int battery1EnablePin = 10;
const int battery2EnablePin = 11;

capacity::measure::measure() {
#ifdef ARDUINO
    digitalWrite(battery1EnablePin, 1);
    digitalWrite(battery2EnablePin, 1);
#endif
    common::time::delay(common::time::ms(1));
}

capacity::measure::~measure() {
#ifdef ARDUINO
    digitalWrite(battery1EnablePin, 0);
    digitalWrite(battery2EnablePin, 0);
#endif
}

const u8 *capacity::measure::data(usize *L) {
    *L = sizeof(m_packed);
    common::analog a1(1);
    m_packed.c1 = a1.read();
    common::analog a2(2);
    m_packed.c2 = a2.read();
    return reinterpret_cast<const u8 *>(&m_packed);
}
