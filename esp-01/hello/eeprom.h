#pragma once
#include <stdint.h>

#ifdef ARDUINO
typedef uint32_t ssize_t;
#else
#include <sys/types.h>
#endif

class eeprom {
public:
  typedef uint8_t* ram_address;
  typedef const uint8_t* const_ram_address; 
  // negative signals error
  typedef ssize_t length;
  eeprom();
  // L is the number of bytes that can be written in ram_address.
  // Returns the number of bytes effectively read.
  length read(const ram_address &dst, const length &L);
  // L is the number of bytes that can be read starting at ram_address.
  // Returns the number of bytes effectively written.
  length write(const const_ram_address &src, const length &L);
  void reset();
  static int test();
};
