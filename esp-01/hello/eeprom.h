#pragma once
#include <stdint.h>
class eeprom {
public:
  typedef uint8_t* ram_address; 
  typedef uint16_t eeprom_address;
  typedef uint16_t length;
  eeprom();
  // L is the number of bytes that can be written in ram_address.
  // Returns the number of bytes effectively read.
  length read(const eeprom_address &src, const ram_address &dst, const length &L);
  // L is the number of bytes that can be read starting at ram_address.
  // Returns the number of bytes effectively written.
  length write(const eeprom_address &dst, const ram_address &src, const length &L);
};
