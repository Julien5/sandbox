#include "debug.h"

#ifdef DEVHOST
void debug::turnBuildinLED(bool on) {
}
#endif

#ifdef ARDUINO
#include "Arduino.h"
void debug::turnBuildinLED(bool on) {
  pinMode(LED_BUILTIN, OUTPUT);
  if (on)
    digitalWrite(LED_BUILTIN,HIGH);
  else
    digitalWrite(LED_BUILTIN,LOW);
}
#endif

#ifdef ESP8266
#include "Arduino.h"
void debug::turnBuildinLED(bool on) {
}
#endif
