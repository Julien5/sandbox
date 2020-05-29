#include "ticksupdater.h"

tickscounter::counter_config config() {
  tickscounter::counter_config ret;
  ret.kMinAloneTicks=0;
  ret.kSecondsUntilAloneTick=3;
  return ret;
}

TicksUpdater::TicksUpdater():counter(config())
{}

bool TicksUpdater::update() {
  if (reader.take()) {
    counter.tick();
    DBG("=> Front detected\n");
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
