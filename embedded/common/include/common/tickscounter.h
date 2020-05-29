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

  constexpr int NTICKS = 20;

  struct packed {    
    packed():m_bins{}{};
    packed(const uint8_t *bytes) {
      *this = *(packed*)bytes;
    }
    bin m_bins[NTICKS];  
    bool operator==(const packed &other) const;
  } __attribute__((packed));

  struct counter_config {
    // tickscounter: delete bin if less than kMinAloneTicks ticks (noise_at_index)
    int kMinAloneTicks = 3;
    // tickscounter: if a bin is older than kSecondsUntilAloneTick, then it is checked if it is noise
    // (noise_at_index)
    int kSecondsUntilAloneTick = 20;
  };
  
  class counter {
    counter_config m_config;
    packed m_packed;
    void tick_first_empty_bin();
    void compress(); 
    int compress_index();
    void denoise();
    void remove_holes();
    
  public:
    counter(const counter_config c=counter_config());
    counter(const uint8_t *data):m_packed(data){}
  
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
    uint8_t bin_count() const;
   
    counter_config config() {
      return m_config;
    }
    const packed *get_packed(size_t * L) const {
      *L=sizeof(m_packed);
      return &m_packed;
    }
  };
  
  int test();
}
