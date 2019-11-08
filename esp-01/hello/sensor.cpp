#include "sensor.h"
#include "clock.h"

// RAII 
template<typename T>
class set_on_return {
  T * const addr;
  const T value;
public:
  set_on_return(T *_addr, const T _value):addr(_addr),value(_value){}
  ~set_on_return() {
    *addr=value;
  }
};

template<>
template<typename T>
class set_on_return<atomic<T>> {
  atomic<T> * const addr;
  const T value;
public:
  set_on_return(atomic<T> *_addr, const T _value):addr(_addr),value(_value){}
  ~set_on_return() {
    //addr->store(value);
    *addr = value;
  }
};

bool sensor::has_ticked() {
  Clock::ms current_time=Clock::millis_since_start();
  Clock::ms time_since_last_rising_reed = current_time-last_time_rising_reed;
  set_on_return<Clock::ms> C(&last_time_rising_reed,current_time);
  set_on_return<atomic<bool>> W(&wake_on_rising_reed,false);

  if (wake_on_rising_reed.load()) {
    if (time_since_last_rising_reed>kAntiBoucingMillis) // avoid interrupt bouncing
      return true;
  }
  return false;
}
