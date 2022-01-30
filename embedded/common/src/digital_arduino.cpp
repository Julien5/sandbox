#include "common/digital.h"
#include "common/debug.h"

using namespace common;

digital::digital(char pin, digital::Mode mode) : m_pin(pin), m_mode(mode) {
    pinMode(m_pin, m_mode == Read ? INPUT : OUTPUT);
};

bool digital::read() {
    if (m_mode != Read) {
        assert(0);
        return false;
    }
    return digitalRead(int(m_pin));
}

void digital::write(bool on) {
    digitalWrite(int(m_pin), on ? 1 : 0);
}
