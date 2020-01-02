#include "rtcmemory.h"

#if !defined(ESP8266)
// TODO: use RAM on atmel.
rtcmemory::rtcmemory() {}

bool rtcmemory::read(uint32_t src_addr, void *des_addr, uint16_t save_size) {
  return false;
}

bool rtcmemory::write(uint32_t des_addr, void *src_addr, uint16_t save_size) {
  return false;
}
#else // ESP8266
#include <espressif/esp_system.h>
#include <stdio.h>
rtcmemory::rtcmemory() {}
// The arduino core uses a 64 byte offset.
// Arduino/cores/esp8266/Esp.cpp: ... system_rtc_mem_read(64 + offset, data, size);
const uint32_t offset=128;
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
    printf("error: bad paramaters ((768 - (src_addr * 4)) < save_size).");
    return false;
  }
  return sdk_system_rtc_mem_read(src_addr, des_addr, save_size);
}

bool rtcmemory::write(void *src_addr, uint16_t save_size) {
  return sdk_system_rtc_mem_write(offset, src_addr, save_size);
}
#endif
