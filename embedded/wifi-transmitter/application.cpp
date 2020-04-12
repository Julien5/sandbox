#include "application.h"

#include "common/debug.h"
#include "common/time.h"
#include "common/wifi.h"
#include "common/utils.h"
#include "common/serial.h"

#include <string.h>

#include "message.h"

std::unique_ptr<serial> S = nullptr;

void transmitter::setup() {
}

#include <chrono>
#include <thread>

int k=0;
void transmitter::loop_serial() {
  if (!S)
    S=std::unique_ptr<serial>(new serial);

  DBG("serial loop\n");

  debug::turnBuildinLED(true);
  while(!S->wait_for_begin())
    DBG("waiting for begin");

  bool ok=false;
  received::message m;
  ok=S->read_until(reinterpret_cast<uint8_t*>(&m.length),sizeof(m.length));
  DBG("read serial: %d\n",m.length);
  if (!ok) {
    DBG("nothing to read.\n");
    return;
  }
  ok=S->read_until(m.data,m.length);
  if (!ok) {
    DBG("failed to read data\n");
    return;
  }
  ok=S->check_end();
  if (!ok) {
    DBG("crc check failed\n");
    return;
  }
  utils::dump(m.data,m.length);
  received::wifi_command cmd = received::read_wifi_command(m);
  /* process cmd */
  cmd;
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  
  /* send result */
  uint8_t data[16]={0};
  uint16_t Ldata=sizeof(data);
  uint8_t status=0;
  S->begin();
  S->write(&status,sizeof(status));
  S->write(reinterpret_cast<uint8_t*>(&Ldata),sizeof(Ldata));
  S->write(data,sizeof(data));
  S->end();
}

