#include "application.h"
#include "debug.h"
#include "time.h"
#include "serial.h"
#include "sync.h"
#include "wifi.h"
#include <string.h>

std::unique_ptr<serial> S =nullptr;
void application::setup() {
  assert(!S);
  S=std::unique_ptr<serial>(new serial);
}

bool read_serial(serial *S, uint8_t * addr, const size_t &L) {
  const uint16_t timeout=10;
  const auto addr0 = addr;
  while((addr-addr0) != int(L)) {
    const auto Lread=S->read(addr,L-(addr-addr0),timeout);
    if (Lread == 0)
      return false;
    addr += Lread;
  }
  return true;
}

int k=0;
void application::loop_serial() {
  DBG("serial loop\n");
  received::message m;

  debug::turnBuildinLED(true);
  bool ok=read_serial(S.get(),reinterpret_cast<uint8_t*>(&m.length),sizeof(m.length));
  assert(ok);
  ok=read_serial(S.get(),m.data,m.length);
  assert(ok);
  global::queue.push(m);
  debug::turnBuildinLED(false);
  time::delay(200);
}

wifi::wifi * W = nullptr;

void application::loop_wifi() {
  bool ok=false;
  received::message rx=global::queue.wait(&ok);
  if (!ok)
    return;
  
  received::wifi_command cmd = received::read_wifi_command(rx);

  if (!W) {
    W=new wifi::wifi();
  }

  if (cmd.command == 'G')
    W->get(cmd.url,0);

  if (cmd.command == 'P')
    W->post(cmd.url,cmd.data,cmd.Ldata,0);
  
  DBG("wifi loop: free=%d ok=%d rx=%s\n",debug::freeMemory(),int(ok),rx.data);
  time::delay(5000);
}


