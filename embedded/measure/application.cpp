#include "application.h"
#include "common/debug.h"
#include "common/time.h"
#include "common/lcd.h"

common::LCD lcd;
void display() {
    for (int d = 0; d < 3; ++d) {
        char msg[17] = {0};
        snprintf(msg, 17, "INIT LCD: %d", d);
        lcd.print(msg);
        common::time::delay(200);
    }
}

void application::setup() {
    debug::init_serial();
    char *c = (char *)malloc(1);
    DBG("%d\r\n", int(c));
}

void application::loop() {
    DBG("hi\r\n");
    debug::turnBuildinLED(true);
    common::time::delay(1000);
    display();
    debug::turnBuildinLED(false);
    common::time::delay(1000);
}
