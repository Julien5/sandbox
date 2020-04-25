#pragma once

#include <stdint.h>
#include "eeprom.h"
#include "clock.h"

#if !defined(ARDUINO) && !defined(ESP8266)
#include <string>
#endif

namespace tickscounter {
  struct bin {
    
    typedef uint16_t count;
    typedef Clock::ms duration;

    bin();
    Clock::ms m_start=0;
    count m_count=0;
    duration m_duration=0;
    void reset();
    bool empty() const;
    Clock::ms end() const;
    void tick();
    void take(bin &other);
    void move(bin &other);
    duration distance(const bin &other) const;
    bool operator==(const bin &other) const;
  } __attribute__((packed));

  // do not forget to change MAGIC if you change this.
  constexpr int NTICKS = 20;

  struct packed {
    typedef int64_t time_since_epoch;
    typedef int32_t time;
    
    packed():m_bins{}{};
    packed(const uint8_t *bytes) {
      *this = *(packed*)bytes;
    }
    bin m_bins[NTICKS];  
    time_since_epoch m_epochtime_at_init=0;
    mutable Clock::ms m_transmission_time=0;
    bool operator==(const packed &other) const;
    std::string json() const;
  } __attribute__((packed));

  struct counter_config {
    // may be quite short, we assume the data are uploaded
    // during the day.
    // tickscounter: do not transmit if activity for less than:
    int kRecentlyActiveSeconds = 20;
    // tickscounter: delete bin if less than kMinAloneTicks ticks (noise_at_index)
    int kMinAloneTicks = 3;
    // tickscounter: if a bin is older than kSecondsUntilAloneTick, then it is checked if it is noise
    // (noise_at_index)
    int kSecondsUntilAloneTick = kRecentlyActiveSeconds;
  };
  
  class counter {
    counter_config m_config;
    packed m_packed;
    void shift_bins(const packed::time_since_epoch t); 
    void tick_first_empty_bin();
    void compress(); 
    int compress_index();
    void denoise();
    void remove_holes();
    
  public:
    counter(const counter_config c=counter_config());
    counter(const uint8_t *data):m_packed(data){}
    void set_epochtime_at_init(const packed::time_since_epoch T0);
  
    void reset();
    void tick();
    void clean();
    void print() const;
    bool is_clean() const;
    bin::count total();
    bool empty() const;  
    Clock::ms last_tick_time();
    Clock::ms age();
    bin getbin(const int &k) const;
    bool recently_active();
    uint8_t bin_count() const;
    uint8_t *getdata(uint16_t * Lout) const;
    
    bool load_eeprom();
    bool save_eeprom_if_necessary();
    counter_config config() {
      return m_config;
    }
    packed get_packed() {
      return m_packed;
    }
  };
  
#if !defined(ARDUINO) && !defined(ESP8266)
  packed fromHex(const std::string &hex);
  std::string asJson(const std::string &hex);
#endif
  
  static void reset_eeprom();
  int test();
}
