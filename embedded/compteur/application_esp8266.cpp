#include "application_esp8266.h"
#include "eeprom.h"
#include <stdio.h>
#include "debug.h"
#include "rtcmemory.h"
#include "sleep.h"
#include "utils.h"

void maintask(void *pvParameters)
{
  printf("maintask\n");
  rtcmemory R;
  int r=rtcmemory::test();
  DBG("r=%d\n",r);
  sleep S;
  S.deep_sleep(500);
  while (true) {};
}
