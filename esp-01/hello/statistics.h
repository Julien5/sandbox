#pragma once

#include <stdint.h>

namespace types
{
  using minute = uint16_t;
  using count = uint8_t;
  using milli = uint32_t;
}

constexpr int NMILLIS=30;
constexpr int NMINUTES=10;
constexpr int NDATA=(sizeof(types::minute)+sizeof(types::count))*NMINUTES
  + sizeof(types::milli)*NMILLIS + 1;

/* 
   as i understand now, packing the data in a char* was not really needed.
   struct alignement on 8-bit arduino yields the same memory layout 
   struct alignement on 32-bit pc yields more memory usage.

   but packing in char is still useful for deserilization on rpi.

   4 294 967 296
*/

class statistics {
private:
  uint8_t data[NDATA];

public:
  statistics();
  statistics(uint8_t * data); // of length NDATA
  //! put all counters to zero
  void reset();
  void tick();
  void save_eeprom() const;
  bool load_eeprom();

  int total() const;
  int minute_count() const;
  void get_minute(const int indx, types::minute *, types::count *) const;
  types::milli get_milli(const int indx) const;
  uint8_t *getdata(int * Lout) const;
  bool operator==(const statistics& other) const;
  static int test();
};
