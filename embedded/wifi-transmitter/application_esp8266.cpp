#include "application.h"
#include "debug.h"
#include "time.h"
#include "serial.h"
#include "sync.h"

#include <string.h>

// serial S;
void application::setup() {
  // S.init();
}

uint8_t rxb[1024]={0};
uint8_t txb[1024]={0};

const uint16_t timeout=10;

int k=0;
void application::loop_serial() {
  DBG("serial loop\n");
  debug::turnBuildinLED(true);
  time::delay(200);
  
  global::queue.push('a'+k);
  k++;
  debug::turnBuildinLED(false);
  time::delay(200);
  /*
  memset(txb,0,sizeof(txb));
  memset(rxb,0,sizeof(rxb));
  auto Lr=S.read(rxb,sizeof(rxb),timeout);
  if (Lr==0) {
    return;
  }  
  sprintf((char*)txb,"(esped)%s",(char*)rxb);
  auto Lw=S.write(txb,strlen((char*)txb));
  */
}

void application::loop_wifi() {
  bool ok=false;
  message rx=global::queue.wait(&ok);
  DBG("wifi loop: ok=%d rx=%c\n",int(ok),rx);
  /*
  memset(txb,0,sizeof(txb));
  memset(rxb,0,sizeof(rxb));
  auto Lr=S.read(rxb,sizeof(rxb),timeout);
  if (Lr==0) {
    return;
  }  
  sprintf((char*)txb,"(esped)%s",(char*)rxb);
  auto Lw=S.write(txb,strlen((char*)txb));
  */
}


