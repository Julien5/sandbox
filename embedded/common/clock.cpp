#include "clock.h"
#include "debug.h"
#include "platform.h"
#include "time.h"

namespace Clock {
  ms millis_since_start() {
    return time::since_reset();
  }
  
  mn minutes_since_start() {
    return millis_since_start()/(1000L*60);
  }
  
  int test() {
    assert(millis_since_start()==0);
    time::delay(1000);
    assert(millis_since_start()==1000);
    return 0;
  }
}
