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
  
  ms millis_at_midnight=0;
  void set_time(char h, char m, char s) {
    ms millis_since_midnight = 1000*(3600*h + 60*m + s);
    millis_at_midnight = since_start() - millis_since_midnight;
  }
  
  constexpr ms millis_24h = 24L*3600*1000;
  
  ms millis_today() {
    ms ret=since_start() - millis_at_midnight;
    if (ret>millis_24h)
      ret-=millis_24h;
    return ret;
  }
}
