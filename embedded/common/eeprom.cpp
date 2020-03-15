#include "eeprom.h"
#include "debug.h"
#include "platform.h"

#ifdef ARDUINO
#include <EEPROM.h>

namespace arduino {
  char read(int addr) {
    return EEPROM.read(addr);
  }
  void write(int addr, char d) {
    EEPROM.write(addr,d);
  }
}
using namespace arduino;
eeprom::eeprom(){};
#endif

#ifdef ESP8266
extern "C" {
#include <spi_flash.h>
}
namespace esp8266 {
  bool dirty=true;
  bool bad=false;
  uint32_t cache[1024] = {0};
  // from esp-open-rtos/tests/cases/08_spiflash.c
  constexpr uint32_t base_addr = 0xF8000; 
  char read(int addr) {
    if (bad) 
      DBG("bad read");
    return char(cache[addr]);
  }
  void write(int addr, char d) {
    cache[addr]=d;
    dirty=true;
  }
  bool commit() {
    if (bad) 
      DBG("bad commit");
    bool ok=spi_flash_write(base_addr,cache, sizeof(cache)/sizeof(char));
    if (ok) 
      dirty=false;
    else
      DBG("bad commit");
    return ok;
  }
}
eeprom::eeprom(){
  bool ok=spi_flash_read(esp8266::base_addr,esp8266::cache,sizeof(esp8266::cache)/sizeof(char));
  if (!ok) {
    DBG("bad eeprom");
    esp8266::bad=true;
  }
}
using namespace esp8266;
#endif

#ifdef DEVHOST
namespace x86 {
  static char mem[1024];
  char read(int addr) {
    if (addr<10)
      printf("read %d : 0x%02x\n",addr,mem[addr] & 0xff);
    return mem[addr];
  }
  void write(int addr, char d) {
    if (addr<10)
      printf("write %d : 0x%02x\n",addr,d & 0xff);
    mem[addr]=d;
  }
}
eeprom::eeprom(){
  // eeprom mock is volatile !
  for(int k=0; k<int(sizeof(x86::mem)/sizeof(char));++k)
    x86::write(0,0);
}
using namespace x86;
#endif

#define MAGIC 0xaa
namespace impl {
  typedef uint16_t eeprom_address;
  uint8_t checksum(uint8_t const * d, const int &L) {
    uint8_t r=0;
    for(int k=0; k<L; ++k) {
      r+=d[k];
    }
    return r;
  }
  // [magic number, uint16_t length, data, uint8_t checksum on data]
  bool eeprom_check(uint16_t *length, int *indx) {
    int index=0;
    uint8_t should_be_magic=read(index++);
    if (should_be_magic!=MAGIC) {
      DBG("first byte is not magic");
      return false;
    }
    if (length)
      *length=0;
    uint16_t L=0;
    {
      char L_[2]={};
      L_[0]=read(index++);
      L_[1]=read(index++);
      L=*(uint16_t*)(&L_);
      if (indx)
	*indx=index;
    }
    DBG("length=%d\n",(int)L);
    uint8_t checksum=0;
    for(int k=0; k<L; ++k,++index) {
      uint8_t d=read(index);
      checksum+=d;
    }
    uint8_t checksum_saved=read(index++);
    if (checksum != checksum_saved) {
      DBG("checkum failed");
      return false;
    }
    if (length)
      *length=L;
    return true;
  }
  eeprom::length Read(const eeprom::ram_address &dst, const eeprom::length &L) {
    uint16_t ret=0;
    int index=0;
    DBG("check eeprom");
    if (!eeprom_check(&ret,&index))
      return -1;
    DBG("OK");
    eeprom::ram_address addr=dst;
    for(int k=0; k<xMin(eeprom::length(ret),L); ++k) {
      *addr=read(index++);
      addr++;
    }
    return ret;
  }
  eeprom::length Write(const eeprom::const_ram_address &src, const eeprom::length &L) {
    int index=0;
    write(index++,MAGIC);
    
    char * L2=(char*)&L;
    // write the length of the data.
    write(index++,*(L2++));
    write(index++,*(L2++));
    
    for(int k=0; k<L; ++index,++k)
      write(index,src[k]);
    
    write(index++,checksum(src,L));
    // TODO check for overflows.
    return L;
  }
}


eeprom::length eeprom::read(const eeprom::ram_address &dst, const eeprom::length &L) {
  return impl::Read(dst,L);
}

eeprom::length eeprom::write(const eeprom::const_ram_address &src, const eeprom::length &L) {
  return impl::Write(src,L);
}

void eeprom::reset() {
  ::write(0,0);
}

#include <string.h>
int eeprom::test() {
  eeprom e;
  const char * data = "hello";

  const size_t Ldata=strlen(data)+1;
  DBG("sizeof(data)=%d\n",Ldata);
 
  e.write((ram_address)data,Ldata);
  uint8_t read_buffer[32]={0};
  DBG("sizeof(read_buffer)=%d\n",sizeof(read_buffer));
  length L=e.read(read_buffer,sizeof(read_buffer));
  if (L<0)
    return 1;
  DBG("L=%d\n",L);
  DBG((char*)read_buffer);
  if (strcmp(data,(char*)read_buffer)!=0)
    return 1;

  // read again
  L=e.read(read_buffer,sizeof(read_buffer));
  if (L<0)
    return 1;

  DBG("test corruption");
  ::write(4,0xba);
  L=e.read(read_buffer,sizeof(read_buffer));
  if (L>=0)
    return 1;
  return 0;
}
