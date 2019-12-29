#pragma once
#include <stdint.h>
#define _NOP() do {} while (0)

#ifndef ARDUINO
#include <string>
#include <stdio.h>

void digitalWrite(uint8_t, uint8_t);
void attachInterrupt(uint8_t interrupt, void ISR(void), uint8_t mode);

#define pinMode(...) _NOP()
#define LED_BUILTIN 13
#define HIGH 0x1
#define LOW  0x0
#define INPUT 0x0
#define OUTPUT 0x1
#define CHANGE 1
#define FALLING 2
#define RISING 3
/* Note:
 * define abs, max and min macros causes problems with
 * stl headers. 
 */
#elif defined(ARDUINO)
#include "Arduino.h"
#endif

template<typename T>
T xMin(const T &a, const T &b) {
  if (a<b)
    return a;
  return b;
}

#if defined(DEVHOST)
using test_ms = uint32_t;
test_ms millis();
void delay(test_ms d);
#elif defined(ESP8266)
// TODO
using test_ms = uint32_t;
test_ms millis();
void delay(test_ms d);
#endif
