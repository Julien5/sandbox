#include "capacity.h"
#include "common/analog.h"
#include "common/time.h"
#include "common/debug.h"

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
    if (L)
        *L = sizeof(m_packed);
    common::analog a1(1);
    common::analog a2(2);
#ifndef SIMULATION
    m_packed.c1 = a1.read();
    m_packed.c2 = a2.read();
    DBG("capacity:%d %d\r\n", int(m_packed.c1), int(m_packed.c2));
#endif
    return reinterpret_cast<const u8 *>(&m_packed);
}
