#include "common/digital.h"

using namespace common;

digital::digital(char pin, digital::Mode mode) : m_pin(pin), m_mode(mode){};

bool digital::read() {
    return false;
}

void digital::write(bool on) {
}
