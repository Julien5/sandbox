#include "common/serial.h"
#include "common/debug.h"
#include "common/utils.h"

const uint8_t kBegin=0xFF;

bool serial::begin() {
  tx_crc8=0x00;
  auto n=write((uint8_t*)&kBegin,1);
  return n==1;
}

bool serial::end() {
  auto n=write((uint8_t*)&tx_crc8,1);
  return n==1;
}
bool serial::read_until(uint8_t * addr, const size_t &L) {
  const uint16_t timeout=100;
  const auto addr0 = addr;
  while((addr-addr0) != int(L)) {
    const auto Lwanted=L-(addr-addr0);
    const auto Lread=read(addr,Lwanted,timeout);
    if (Lread<0)
      return false;
    addr += Lread;
  }
  return true;
}

bool serial::wait_for_begin() {
  while(true) {
    rx_crc8=0x00;
    uint8_t begin=0;
    bool ok=read_until(&begin,sizeof(begin));
    if (!ok)
      continue;
    if (begin == kBegin)
      break;
    // DBG("waiting for begin (received 0x%02x)\n",begin);
  }
  return true;
}

bool serial::check_end() {
  // reading modifies rx_crc8
  // so we must save the last value to check it against the transmitted crc.
  // alternatively, we could check that the crc is null 
  uint8_t crc8_received=0;
  auto saved_rx_crc8 = rx_crc8;
  while(true) {
    bool ok=read_until(reinterpret_cast<uint8_t*>(&crc8_received),sizeof(crc8_received));
    if (ok)
      break;
  }
  if (crc8_received != saved_rx_crc8)
    DBG("CRC: 0x%02x != 0x%02x\r\n",crc8_received,saved_rx_crc8);
  auto match = crc8_received == saved_rx_crc8;
  if (!match) {
    assert(0);
  } 
  return match;
}

