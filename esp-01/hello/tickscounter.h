#pragma once
#include <stdint.h>
#ifndef ARDUINO
#include <string>
#endif

struct bin {
  typedef uint16_t time;
  typedef uint16_t count;
  typedef uint16_t duration;
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
  tickscounter(const uint8_t *addr);
  void reset();
#ifndef ARDUINO
  static tickscounter fromHex(const std::string &hex);
  std::string json() const;
  static std::string asJson(const std::string &hex);
#endif
  void tick();
  void clean();
  void print() const;
  bool is_clean() const;
  bin::count total();
  bool empty() const;  
  bin::time last_tick_time();
  bin::time age();
  bool recently_active();
  uint8_t bin_count() const;
  uint8_t *getdata(int * Lout) const;
  bool operator==(const tickscounter &other) const;
  static int test();
} __attribute__((packed));
