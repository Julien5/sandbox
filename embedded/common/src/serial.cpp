#include "common/serial.h"
#include "common/debug.h"

const uint8_t kBegin=0xFF;

bool serial::begin() {
  auto n=write((uint8_t*)&kBegin,1);
  return n==1;
}

bool serial::read_until(uint8_t * addr, const size_t &L) {
  const uint16_t timeout=10;
  const auto addr0 = addr;
  while((addr-addr0) != int(L)) {
    const auto Lread=read(addr,L-(addr-addr0),timeout);
    if (Lread == 0)
      return false;
    addr += Lread;
  }
  return true;
}

bool serial::wait_for_begin() {
  while(true) {
    uint8_t begin=0;
    bool ok=read_until(reinterpret_cast<uint8_t*>(&begin),sizeof(begin));
    if (!ok)
      continue;
    DBG("begin? 0x%02x\n",begin);
    if (begin == kBegin)
      break;
  }
  return true;
}

