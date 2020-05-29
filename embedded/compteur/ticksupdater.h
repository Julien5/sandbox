#pragma once

#include "common/tickscounter.h"
#include "histogram.h"

class TicksReader {
  histogram::Histogram H;
  bool m_last_value=0;
 public:
  bool take();
};

class TicksUpdater {
  tickscounter::counter counter;
  TicksReader reader;
public:
  bool update();
  void print();
  static int test();
};
