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
  duration distance(const bin &other) const;
  bool operator==(const bin &other) const;
} __attribute__((packed));

constexpr int NTICKS = 20;

class tickscounter {
  bin m_bins[NTICKS];
  mutable bin::time m_tranmission_time=0;
  char version=1;
 
  bool tick_if_possible();
  void compress(); 
  int compress_index();
  void denoise();
  void remove_holes();
public:
  tickscounter();
  tickscounter(const uint8_t *addr, const int L);
  void tick();
  void clean();
  void print() const;
  bool is_clean() const;
  bin::count total() const;
  uint8_t *getdata(int * Lout) const;
  bool operator==(const tickscounter &other) const;
  static int test();
} __attribute__((packed));
