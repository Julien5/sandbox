#pragma once
#include <stdint.h>
#define _NOP() do {} while (0)

#ifndef ARDUINO
#include <string>
#include <stdio.h>
#elif defined(ARDUINO)
#include "Arduino.h"
#endif

/* Note:
 * define abs, max and min macros causes problems with
 * stl headers. 
 */

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
