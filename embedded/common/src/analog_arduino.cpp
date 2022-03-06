#include "common/analog.h"
#include "common/debug.h"
#include "Arduino.h"

class analog_init {
  public:
    analog_init() {
        // DEFAULT: the default analog reference of 5 volts (on 5V Arduino boards) or 3.3 volts (on 3.3V Arduino boards)
        // INTERNAL: a built-in reference, equal to 1.1 volts on the ATmega168 or ATmega328P and 2.56 volts on the ATmega32U4 and ATmega8 (not available on the Arduino Mega)
        analogReference(INTERNAL);
        //analogReference(DEFAULT);
    }
};

u16 common::analog::read() {
    static analog_init i;
    auto ret = analogRead(m_pin);
    //DBG("common::analog::read:%d\r\n", int(ret));
    return ret;
}
