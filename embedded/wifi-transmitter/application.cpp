#include "application.h"

#include "common/debug.h"
#include "common/time.h"
#include "common/wifi.h"
#include "common/utils.h"
#include "common/serial.h"

#include <string.h>

#include "message.h"

std::unique_ptr<serial> S = nullptr;
#ifdef DEVHOST
#include "wifi_curl.h"
#define WIFI wifi_curl
#else
#include "common/wifi.h"
#define WIFI wifi
#endif
namespace {
  std::unique_ptr<wifi::WIFI> W = nullptr;
}

void transmitter::setup() {
}

#include <chrono>
#include <thread>

class serial_callback : public wifi::callback {
  serial * output;
public:
  serial_callback(serial * _s):output(_s){};
  void status(uint8_t s) {
    DBG("forwarding status %d \n",int(s));
    assert(s==0);
    output->write(&s,sizeof(s));
  }
  
  void data_length(uint16_t total_length) {
    DBG("forwarding total %d bytes\n",total_length);
    assert(total_length>0);
    output->write(reinterpret_cast<uint8_t*>(&total_length),sizeof(total_length));
  }
  void data(uint8_t * data, size_t length) {
    DBG("forwarding %d bytes\n",length);
    output->write(data,length);
  }
  void crc(bool ok) {
    DBG("forwarding crc %d \n",int(ok));
    assert(ok);
  }
};

int k=0;
void transmitter::loop_serial() {
  if (!S)
    S=std::unique_ptr<serial>(new serial);
  if (!W)
    W=std::unique_ptr<wifi::WIFI>(new wifi::WIFI);
  
  DBG("serial loop\n");

  /*
    S->begin();
    uint8_t d[4]={0x01,0x02,0x03,0x04};
    S->write(d,sizeof(d));
    S->end();
    Time::delay(1000);
    return;
  */
  DBG("waiting for begin");
    
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
  DBG("process %s\n",cmd.url);
  S->begin();
  serial_callback cb(S.get());
  if (cmd.command == 'G')
    W->get(cmd.url,&cb);
  if (cmd.command == 'P') {
    DBG("%d\n",cmd.Ldata);
    utils::dump(cmd.data,cmd.Ldata);
    W->post(cmd.url,cmd.data,cmd.Ldata,&cb);
  }
  DBG("end\n");
  S->end();
}

