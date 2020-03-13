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

  message m;
  m.data[0] = 'a'+k;
  m.data[1] = 'A'+k;
  m.data[2] = '0'+k;
  
  global::queue.push(m);
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
  DBG("wifi loop: free=%d ok=%d rx=%s\n",debug::freeMemory(),int(ok),rx.data);
}


