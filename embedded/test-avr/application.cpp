#include "application.h"

void application::setup() {
    Serial.begin(115200);
    // The default is 8 data bits, no parity, one stop bit.
    Serial.println("<Arduino is ready>");
}

void application::loop() {
    Serial.println("<loop>");
    delay(25);
}
