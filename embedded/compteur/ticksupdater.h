#pragma once

#include "common/tickscounter.h"

class TicksReader {
 public:
  bool take();
};

class TicksUpdater {
  tickscounter::counter counter;
  TicksReader reader;
public:
  static int test();
};
