#include "common/analog.h"
#include "Arduino.h"

class analog_init {
  public:
    analog_init() {
        analogReference(INTERNAL);
    }
};

u16 common::analog::read() {
    static analog_init i;
    return analogRead(0);
}
