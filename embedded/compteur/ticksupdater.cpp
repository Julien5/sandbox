#include "ticksupdater.h"
#include "common/debug.h"
#include "common/time.h"

tickscounter::counter_config config() {
  tickscounter::counter_config ret;
  ret.kMinAloneTicks=0;
  ret.kSecondsUntilAloneTick=3;
  return ret;
}

TicksUpdater::TicksUpdater():counter(config())
{}

bool TicksUpdater::update() {
  const auto t = Time::since_reset();
  const double hours = double(t)*0.001/3600;
  if (hours>0) {
    DBG("hours:%2.2f total:%6d power:%2.3fkW\n",hours,counter.total(),counter.total()/hours/75);
  }
  if (reader.take()) {
    counter.tick();
    return true;
  }
  return false;
}

void TicksUpdater::print() {
  
}

int TicksUpdater::test() {
  TicksUpdater U;
  while(true) {
    if (U.update())
      U.print();
  }
}
