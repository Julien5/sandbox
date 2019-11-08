#pragma once
#include <stdint.h>
#define _NOP() do {} while (0)

#ifndef ARDUINO
#include <string>
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
#ifdef abs
#undef abs
#endif
#define abs(x) ((x)>0?(x):-(x))

#ifdef max
#undef max
#endif
#define max(a,b) ((a)>(b)?(a):(b))

#ifdef min
#undef min
#endif
#define min(a,b) ((a)<(b)?(a):(b))

#endif


#ifdef ARDUINO
template<typename T>
class atomic {
  private T value;
public:
  atomic(T v):value(v){};
  T load() const {
    return t;
  }
  T operator=(T val) noexcept {
    value=val;
  }
}
#else
#include <atomic>
  template <typename T>
  using atomic = std::atomic<T>;
#endif
