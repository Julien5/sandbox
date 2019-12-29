#ifndef ARDUINO

#ifdef DEVHOST
#include <iostream>
#endif

#include "platform.h"

MockSerial Serial;

void MockSerial::begin(unsigned long baud) {
}

template <typename S>
void printline(S s) {
  #ifdef DEVHOST
  std::cout << s << std::endl;
  #endif
}

template <typename S>
void print(S s) {
  #ifdef DEVHOST
  std::cout << s;
  #endif
}

void MockSerial::println(const std::string &s) {
  printline(s);
}

void MockSerial::println(uint32_t s) {
  printline(s);
}

void MockSerial::print(const std::string &s) {
  print(s);
}

uint32_t MockSerial::available() const {
  return 0;
}

int MockSerial::readBytes(char *buffer, const int length) {
  return 0;
}

int MockSerial::write(const char *buffer, int length) {
  return 0;
}

void MockSerial::flushInput() {
}

void digitalWrite(uint8_t, uint8_t) {
}

void attachInterrupt(uint8_t interrupt, void ISR(void), uint8_t mode) {
}
#endif

#ifndef ARDUINO
#include <atomic>
std::atomic<test_ms> test_t(0);
test_ms millis() {
  return test_t.load(); 
}
void delay(test_ms d) {
  test_t+=d;
}
#endif
