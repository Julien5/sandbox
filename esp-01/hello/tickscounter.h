#pragma once
#include <stdint.h>

struct bin {
  using time = uint16_t;
  using count = uint16_t;
  using duration = uint16_t;
  bin();
  time m_start=0;
  count m_count=0;
  duration m_duration=0;
  void reset();
  bool empty() const;
  time end() const;
  bool accepts() const;
  bool tick();
  void take(bin &other);
  void move(bin &other);
  duration distance(const bin &other);
  
};

constexpr int NTICKS = 20;

class tickscounter {
  bin m_bins[NTICKS];
  bool tick_if_possible();
  void compress();
  int m_compress_index=-1;
  bin::duration m_dmin=0;
  void update_compress_index();
public:
  tickscounter();
  void tick();
  void clean();
  void print() const;
  bool is_clean() const;
  bin::count total() const;
  static int test();
};
