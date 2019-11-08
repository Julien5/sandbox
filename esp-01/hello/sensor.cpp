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
    *addr = value;
  }
};

static sensor *addr=nullptr;
#ifdef ADUINO
void on_rising_reed() {
  addr->on_rising_reed();
}
void start_sensor() {
  attachInterrupt(0,on_rising_reed,RISING);
}
#else
#undef abs
#undef min
#undef max

#include <random>
int random_number() {
  std::random_device rd;     // only used once to initialise (seed) engine
  std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
  std::uniform_int_distribution<int> uni(0,100);
  return uni(rng);
}

#include <future>
void generate_interrupts() {
  static int count=0;
  while(true) {
    int r=kAntiBoucingMillis+random_number();
    if (count%10 == 0)
      r+=2000;
    std::this_thread::yield();
    std::this_thread::sleep_for(std::chrono::milliseconds(r));
    delay(r);
    if (count<50)
      addr->on_rising_reed();
    count++;
  }
}
void start_sensor() {
  static auto fut=std::async(generate_interrupts);
}
#endif

sensor::sensor():wake_on_rising_reed(false) {
  addr=this;
  start_sensor();
}

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
