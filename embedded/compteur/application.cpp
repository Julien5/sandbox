#include "application.h"
#include "eeprom.h"
#include "debug.h"
#include "rtcmemory.h"
#include "sleep.h"
#include "utils.h"
#include "platform.h"
#include "time.h"
#include "transient.h"

#include <inttypes.h>

TRANSIENT transient tr;
void application::setup() {
  tr.n_wakeup++;
  time::set_current_epoch(tr.n_wakeup*1000);
}

void application::loop()
{
  DBG("#wakeup=%d\n",tr.n_wakeup);
  int n=5;
  while(n--) {
    DBG("time since reset=%d\n",time::since_reset());
    DBG("time since epoch=%d\n", int(time::since_epoch()));
  }
  if (!transient::save(&tr))
    DBG("error: could not write transient!\n");
  sleep().deep_sleep(1000);
}
