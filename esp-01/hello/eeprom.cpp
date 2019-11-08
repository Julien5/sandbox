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
  if (addr<10)
    printf("[read %d : %#02x]\n",addr,mem[addr]);
  return mem[addr];
#endif
}

void eeprom::write(int addr, char d) {
#ifdef ARDUINO
  EEPROM.write(addr,d);
#else
  if (addr<10)
    printf("write %d : %#02x\n",addr,d);
  mem[addr]=d;
#endif
}   
