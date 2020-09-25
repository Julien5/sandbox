#include "common/analog.h"
#include "Arduino.h"

u16 common::analog::read() {
    return analogRead(0);
}
