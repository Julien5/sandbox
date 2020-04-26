#include "ticksupdater.h"

bool TicksUpdater::update() {
  if (reader.take()) {
    counter.tick();
    DBG("=> Front detected\n");
    return true;
  }
  DBG("no front detected\n");
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
