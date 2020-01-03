#include "rtcmemory.h"
#include "debug.h"

#if !defined(ESP8266)
// TODO: use RAM on atmel.
rtcmemory::rtcmemory() {}

bool rtcmemory::read(void *des_addr, uint16_t save_size) {
  return false;
}

bool rtcmemory::write(void *src_addr, uint16_t save_size) {
  return false;
}
#else // ESP8266
#include <espressif/esp_system.h>
#include <stdio.h>
rtcmemory::rtcmemory() {}
// The arduino core uses a 64 byte offset...
// Arduino/cores/esp8266/Esp.cpp: ... system_rtc_mem_read(64 + offset, data, size);
// ... but the first 9 bytes after 64 seems buggy:
// https://github.com/esp8266/Arduino/issues/619
const uint32_t offset=73;
bool rtcmemory::read(void *des_addr, uint16_t save_size) {
  const uint32_t src_addr = offset+0;
  if (src_addr > 191) {
    printf("error: src_addr:%d\n",src_addr);
    return false;
  }
  if ((intptr_t)des_addr & 3) {
    printf("error: destination addr is not 4-byte aligned.");
    return false;
  }
  if ((768 - (src_addr * 4)) < save_size) {
    printf("error: bad paramaters ((768 - (src_addr * 4)) < save_size):\n");
    printf("error: \t(768 - (src_addr * 4))=%d\n",768 - (src_addr * 4));
    printf("error: \tsave_size=%d\n",save_size);
    printf("error: cannot read so much data, data_size should be <= 476\n");
    return false;
  }
  return sdk_system_rtc_mem_read(src_addr, des_addr, save_size);
}

bool rtcmemory::write(void *src_addr, uint16_t save_size) {
  return sdk_system_rtc_mem_write(offset, src_addr, save_size);
}
#endif
#include "utils.h"
int rtcmemory::test() {
  DBG("rtcmemory::test()\n");
  rtcmemory R;
  constexpr size_t LEN=512-36;
  unsigned char data0[LEN]={0};
  if (!R.read(data0,sizeof(data0))) {
    DBG("could not read\n");
    return 1;
  }
  utils::dump(data0,LEN);
  char data1[LEN]={0};
  for(unsigned k=0;k<LEN;++k)
    data1[k]=0xFF;
  if (!R.write(data1,sizeof(data1))) {
    DBG("could not write\n");
    return 2;
  }
  return 0;
}
