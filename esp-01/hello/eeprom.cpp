#include "eeprom.h"
#include "debug.h"

#ifdef ARDUINO
#include <EEPROM.h>
#else
static char mem[512];
#endif

char eeprom::read(int addr) {
#ifdef ARDUINO
  return EEPROM.read(addr);
#else
  return mem[addr];
#endif
}

void eeprom::write(int addr, char d) {
#ifdef ARDUINO
  EEPROM.write(addr,d);
#else
  debug(addr);
  debug(char(d));
  mem[addr]=d;
#endif
}   
