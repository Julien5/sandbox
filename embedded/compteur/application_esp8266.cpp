#include "application_esp8266.h"
#include "eeprom.h"
#include <stdio.h>
#include "debug.h"
#include "rtcmemory.h"
#include "sleep.h"

void maintask(void *pvParameters)
{
  printf("maintask\n");
  // printf("eeprom test: %d\n",eeprom::test());
  rtcmemory R;
  char data0[12]={0};
  if (!R.read(data0,sizeof(data0)))
    printf("could not read\n");
  printf("data0:%s\n",data0);
  char data1[12]="bonjour";
  printf("data1:%s\n",data1);
  if (!R.write(data1,sizeof(data1)))
    printf("could not write\n");
  sleep S;
  S.deep_sleep(500);
  while (true) {};
}
