#include "softdebug.h"
#include "common/debug.h"

const int debugEnablePin = 7;

void softdebug::init() {
#ifdef ARDUINO
    pinMode(debugEnablePin, INPUT);
#endif
}

bool softdebug::log_enabled() {
#ifdef ARDUINO
    return digitalRead(debugEnablePin) == LOW;
#endif
    return true;
}
