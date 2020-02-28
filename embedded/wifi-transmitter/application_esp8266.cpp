#include "application.h"
#include "debug.h"
#include "time.h"
#include "serial.h"

#include <string.h>

serial S;
void application::setup() {
   S.init();
}

uint8_t rxb[1024]={0};
uint8_t txb[1024]={0};

const uint16_t timeout=10;

void application::loop() {
  /*debug::turnBuildinLED(true);
  time::delay(200);
  debug::turnBuildinLED(false);
  time::delay(200);
  */
  memset(txb,0,sizeof(txb));
  memset(rxb,0,sizeof(rxb));
  auto Lr=S.read(rxb,sizeof(rxb),timeout);
  if (Lr==0) {
    return;
  }  
  sprintf((char*)txb,"(esped)%s",(char*)rxb);
  auto Lw=S.write(txb,strlen((char*)txb));
}


