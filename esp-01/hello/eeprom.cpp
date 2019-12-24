#include "eeprom.h"
#include "debug.h"
#include "platform.h"

#if !defined(ARDUINO) && !defined(ESP8266)
#define DEVHOST
#endif

#ifdef ARDUINO
#include <EEPROM.h>
// [magic number, length16, data, checksum on data]

namespace arduino {
  char readbyte(int addr) {
    return EEPROM.read(addr);
  }
  void writebyte(int addr, char d) {
    EEPROM.write(addr,d);
  }
}
using namespace arduino;
eeprom::eeprom(){};
#endif

#ifdef DEVHOST

namespace x86 {
  static char mem[4096];
  char readbyte(int addr) {
    if (addr<10)
      printf("read %d : %#02x\n",addr,mem[addr]);
    return mem[addr];
  }
  void writebyte(int addr, char d) {
    if (addr<10)
      printf("write %d : %#02x\n",addr,d);
    mem[addr]=d;
  }
}
eeprom::eeprom(){
  // eeprom mock is volatile !
  for(int k=0; k<sizeof(x86::mem)/sizeof(char);++k)
    x86::writebyte(0,0);
}
using namespace x86;
#endif

#if defined (ARDUINO) || defined (DEVHOST)
#define BYTEWISE
#define MAGIC 86
namespace bytewise {
  bool eeprom_check(uint16_t *length, int *indx) {
    int index=0;
    uint8_t should_be_magic=readbyte(index++);
    if (should_be_magic!=MAGIC)
      return false;
    if (length)
      *length=0;
    uint16_t L=0;
    {
      char _L[2]={};
      _L[0]=readbyte(index++);
      _L[1]=readbyte(index++);
      L=*(uint16_t*)(&_L);
      if (indx)
	*indx=index;
    }
    uint8_t checksum=0;
    for(int k=0; k<L; ++k,++index) {
      uint8_t d=readbyte(index);
      checksum+=d;
    }
    uint8_t checksum_saved=readbyte(index++);
    if (checksum != checksum_saved)
      return false;
    if (length)
      *length=L;
    return true;
  }
  eeprom::length read(const eeprom::ram_address &dst, const eeprom::length &L) {
    const eeprom::eeprom_address src=0;
    eeprom::length ret=0;
    int index=0;
    if (!eeprom_check(&ret,&index))
      return 0;
    eeprom::ram_address addr=dst;
    for(int k=0; k<xMin(ret,L); ++k) {
      *addr=readbyte(index++);
      addr++;
    }
    return ret;
  }
  eeprom::length write(const eeprom::ram_address &src, const eeprom::length &L) {
    const eeprom::eeprom_address dst=0;
    int index=0;
    writebyte(index++,MAGIC);
    
    char * _L=(char*)&L;
    writebyte(index++,*(_L++));
    writebyte(index++,*(_L++));
    
    for(int k=0; k<L; ++index,++k)
      writebyte(index,src[k]);
    //writebyte(index++,checksum(src,L));
    return L;
  }
}
using namespace bytewise;
#endif

#ifdef ESP8266
// use directly: spiflash_write from spiflash.h
#include <sysparam.h>
using namespace esp8266;
namespace esp8266 {
  eeprom::eeprom() {
  }
  eeprom::length read(const eeprom::ram_address &dst, const eeprom::length &L) {
    return 0;
  }
  eeprom::length write(const eeprom::ram_address &src, const eeprom::length &L) {
  }
}
#endif

eeprom::length eeprom::read(const eeprom_address &src, const eeprom::ram_address &dst, const eeprom::length &L) {
  return bytewise::read(dst,L);
}

eeprom::length eeprom::write(const eeprom_address &dst, const eeprom::ram_address &src, const eeprom::length &L) {
  return bytewise::write(src,L);
}
