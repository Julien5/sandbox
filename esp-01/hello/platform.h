#pragma once
#include <stdint.h>
#define _NOP() do {} while (0)

#ifndef ARDUINO
#include <string>
#include <iostream>
class MockSerial {
public:
  void begin(unsigned long baud);
  void println(const std::string &);
  void println(uint32_t);
  void print(const std::string &);
  
  uint32_t available() const;
  int readBytes(char *buffer, const int length);
  int write(const char *buffer, int length=-1);
  void flushInput();
    
};
extern MockSerial Serial;
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
#else
#include "Arduino.h"
#endif

template<typename T>
T xMin(const T &a, const T &b) {
  if (a<b)
    return a;
  return b;
}

#ifdef ARDUINO
template<typename T>
class Atomic {
  T value;
public:
  Atomic(T v):value(v){};
  T load() const {
    return value;
  }
  T operator=(T val) noexcept {
    value=val;
    return value;
  }
};
#else
#include <atomic>
  template <typename T>
  using Atomic = std::atomic<T>;
#endif

#ifndef ARDUINO
using test_ms = uint32_t;
test_ms millis();
void delay(test_ms d);
#endif
