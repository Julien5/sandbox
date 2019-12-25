#pragma once

#include <stdint.h>
#include "eeprom.h"

#ifndef ARDUINO
#include <string>
#endif

namespace config {
  // may be quite short, we assume the data are uploaded
  // during the day.
  // tickscounter: do not transmit if activity for less than:
  constexpr int kRecentlyActiveSeconds = 20;
  // tickscounter: clean bin if kMinAloneTicks ticks or less (noise_at_index)
  constexpr int kMinAloneTicks = 2;
  // tickscounter: if a bin is older than kSecondsUntilAloneTick, then it is checked if it is noise
  // (noise_at_index)
  constexpr int kSecondsUntilAloneTick = kRecentlyActiveSeconds;
}

struct bin {
  typedef int32_t time;
  typedef uint16_t count;
  typedef uint32_t duration;

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

// do not forget to change MAGIC if you change this.
constexpr int NTICKS = 20;

class tickscounter {
public:
  typedef int64_t time_since_epoch;
  
private:  
  time_since_epoch m_epochtime_at_init=0;
  void shift_bins(const time_since_epoch t); 

  
  bin m_bins[NTICKS];
  mutable bin::time m_transmission_time=0;
  

  
  bool tick_if_possible();
  void compress(); 
  int compress_index();
  void denoise();
  void remove_holes();
  static void reset_eeprom();
public:
  tickscounter();
  tickscounter(const uint8_t *addr);
  void set_epochtime_at_init(const time_since_epoch T0);
  
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
  bin getbin(const int &k) const;
  bool recently_active();
  uint8_t bin_count() const;
  uint8_t *getdata(uint16_t * Lout) const;
  bool operator==(const tickscounter &other) const;
  static int test();
  bool load_eeprom();
  bool save_eeprom_if_necessary();
} __attribute__((packed));
