#include "application.h"
#include "common/debug.h"
#include "common/time.h"
#include "LiquidCrystal.h"

LiquidCrystal disp(7, 5, 6, 10, 11, 12);

void lcd() {
    for (int d = 0; d < 3; ++d) {
        char msg[17] = {0};
        snprintf(msg, 17, "INIT LCD: %d", d);
        disp.setCursor(0, 0);
        disp.print(msg);
        Time::delay(200);
    }
}

void application::setup() {
    debug::init_serial();
    char *c = malloc(1);
    DBG("%d\r\n", int(c));
    disp.begin(16, 2);
}

void application::loop() {
    DBG("hi\r\n");
    debug::turnBuildinLED(true);
    Time::delay(1000);
    lcd();
    debug::turnBuildinLED(false);
    Time::delay(1000);
}
