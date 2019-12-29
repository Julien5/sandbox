#include "sensor.h"
#include "clock.h"
#include "platform.h"

// RAII
template<typename A, typename V>
class set_on_return {
  A const addr;
  const V value;
public:
  set_on_return(A _addr, const V _value):addr(_addr),value(_value){}
  ~set_on_return() {
    *addr=value;
  }
};

template<typename U, typename V>
set_on_return<U,V> make_set_on_return(U u, V v) {
  return set_on_return<U,V>(u,v);
}		     

static sensor *addr=nullptr;
#if defined(ARDUINO) || defined(ESP8266)
void on_rising_reed() {
  addr->on_rising_reed();
}
const int reed_pin = 2;
void start_sensor() {
  pinMode(reed_pin, INPUT_PULLUP);
  attachInterrupt(0,on_rising_reed,RISING);
}
#else
#include <random>
int random_number() {
  std::random_device rd;     
  std::mt19937 rng(rd());
  std::uniform_int_distribution<int> uni(100,750);
  return uni(rng);
}

#include <future>
void smalldelay(int count) {
  // genarates bursts of 17 ticks in about 17-20 secs
  int d=kAntiBoucingMillis+random_number();
  if (count%17 == 0)
    d+=2000;
  delay(d);
}

void generate_interrupts() {
  static int count=0;
  while(true) {
    if (count<15000) {
      addr->on_rising_reed();
    }
    count++;
    smalldelay(count);
    std::this_thread::yield();
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
  if (!wake_on_rising_reed.load())
    return false;
  
  Clock::ms current_time=Clock::millis_since_start();
  Clock::ms time_since_last_rising_reed = current_time-last_time_rising_reed;

  auto C=make_set_on_return(&last_time_rising_reed,current_time);
  auto W=make_set_on_return(&wake_on_rising_reed,false);
  if (time_since_last_rising_reed>kAntiBoucingMillis) // avoid interrupt bouncing
    return true;
  return false;
}
