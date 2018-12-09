#pragma once

class eeprom {
public:
  char read(int addr);
  void write(int addr, char d);
};
