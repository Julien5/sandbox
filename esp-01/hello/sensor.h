#pragma once

#include "clock.h"
#include "defines.h"

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

class sensor {
  Clock::ms last_time_rising_reed=0;
  atomic<bool> wake_on_rising_reed;
public:
  sensor():wake_on_rising_reed(false){}    
  void on_rising_reed() {
    wake_on_rising_reed.store(true);
  }    
  bool has_ticked();
};
