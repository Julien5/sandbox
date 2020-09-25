#include "application.h"
#include "common/debug.h"
#include "common/time.h"
#include "common/lcd.h"
#include "analog.h"

common::LCD lcd;

u16 analog_read() {
#ifdef PC
    return analog().read();
#else
    return common::analog().read();
#endif
}

void display() {
    const auto a = analog_read();
    char msg[17] = {0};
    snprintf(msg, 17, "value: %4d", a);
    DBG("a=%d\r\n", a);
    lcd.print(msg);
}

void application::setup() {
    debug::init_serial();
    char *c = (char *)malloc(1);
    DBG("%d\r\n", int(c));
}

void application::loop() {
    debug::turnBuildinLED(true);
    display();
    debug::turnBuildinLED(false);
    common::time::delay(250);
}
