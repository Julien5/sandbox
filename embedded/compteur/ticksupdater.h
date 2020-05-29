#pragma once

#include "common/tickscounter.h"
#include "histogram.h"

class TicksReader {
  histogram::Histogram H;
  bool m_last_value=0;
  bool calibrated(uint16_t * TL, uint16_t * TH) const;
 public:
  // returns 1 if the adc just got high.
  bool take();
};

class TicksUpdater {
  tickscounter::counter counter;
  TicksReader reader;
public:
  TicksUpdater();
  bool update();
  void print();
  static int test();
};
