#include "clock.h"
#include "debug.h"

namespace Clock {
  ms sleep_millis = 0;
  void wake_up_after(ms m) {
    sleep_millis += m;
  }
  
  ms since_start() {
    return millis() + sleep_millis;
  }
  
  int64_t millis_at_midnight=0;
  void set_time(char h, char m, char s) {
    sleep_millis=0;
    ms millis_since_midnight = 1000L*(3600L*h + 60L*m + s);
    int64_t ss = since_start();
    millis_at_midnight = ss  - millis_since_midnight;
  }

  bool good() {
    if (millis_at_midnight==0)
      return false;
    return true;
  }
  
  constexpr ms millis_24h = 24L*3600*1000;
  
  ms millis_today() {
    ms ret=since_start() - millis_at_midnight;
    if (ret>millis_24h)
      ret-=millis_24h;
    return ret;
  }
  
  int test() {
    set_time(0,0,1);
    assert(millis_today()==1000);
    delay(1000);
    assert(millis_today()==2000);
    set_time(0,1,0);
    assert(millis_today()==60*1000L);
    set_time(1,0,0);
    assert(millis_today()==3600*1000L);
    debug("clock is good");
    return 0;
  }
}
