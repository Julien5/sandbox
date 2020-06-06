#include "application.h"
#include "common/debug.h"
#include "common/time.h"

#ifdef ARDUINO
#include "Arduino.h"
void application::setup() {
  Serial.begin(9600);
}
#else
void application::setup() {
}
#endif

void application::loop() {
  DBG("hi\r\n");
  debug::turnBuildinLED(true);
  Time::delay(1000);
  debug::turnBuildinLED(false);
  Time::delay(1000);
}


