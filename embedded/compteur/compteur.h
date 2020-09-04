#pragma once

#include "common/tickscounter.h"
#include "histogram.h"

class TicksReader {
  histogram::Histogram H;
  bool m_last_value=0;
  bool calibrated(u16 * TL, u16 * TH) const;
 public:
  // returns 1 if the adc just got high.
  bool take();
};

class compteur {
  tickscounter::counter counter;
  TicksReader reader;
public:
  compteur();
  bool update();
  void print();
  tickscounter::bin::count total();
  
  const u8* data(size_t *L) const;
  static int test();
};
