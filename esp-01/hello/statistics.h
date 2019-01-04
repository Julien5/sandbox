#pragma once

#include <cstdint>

namespace types
{
  using minute = uint16_t;
  using count = uint8_t;
  using milli = uint16_t;
}

constexpr int NMILLIS=2;
constexpr int NMINUTES=3;
constexpr int NDATA=(sizeof(types::minute)+sizeof(types::count))*NMINUTES
  + sizeof(types::milli)*NMILLIS + 1;

/* 
   as i understand now, packing the data in a char* was not really needed.
   struct alignement on 8-bit arduino yields the same memory layout 
   struct alignement on 32-bit pc yields more memory usage.

   but packing in char is still useful for deserilization on rpi.
*/

class statistics {
private:
  uint8_t data[NDATA];
  void clear();

public:
  statistics();
  statistics(uint8_t * data); // of length NDATA
  void tick();
  void save_eeprom();
  bool load_eeprom();
  int total();
  int minute_count();
  void get_minute(const int m, types::minute *, types::count *);
  uint8_t *getdata(int * Lout);
  bool operator==(const statistics& other);
  static int test();
};
