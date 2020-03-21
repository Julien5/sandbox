#include "application.h"

#include "common/debug.h"
#include "common/time.h"
#include "common/wifi.h"
#include "common/utils.h"

#include "common/serial.h"
#include "sync.h"

#include <string.h>

std::unique_ptr<serial> S = nullptr;
std::unique_ptr<wifi::wifi> W = nullptr;

void application::setup() {
  assert(!S);
  S=std::unique_ptr<serial>(new serial);
  assert(!W);
  W=std::unique_ptr<wifi::wifi>(new wifi::wifi());
}

bool read_serial(serial *S, uint8_t * addr, const size_t &L) {
  return S->read_until(addr,L);
}

const uint8_t kBegin=0xFF;
#ifdef DEVHOST
void application::loop_arduino() {
  const char command = 'G';
  const char * url = "http://foo.bar/xx";
  const char * data = nullptr;
  const uint16_t Ldata = 0;
  //                G   http......    0 + Ldata + data
  uint16_t Ltotal = 1 + strlen(url) + 1 + sizeof(Ldata) + Ldata;


  S->write((uint8_t*)&kBegin,1);
  S->write((uint8_t*)&Ltotal,sizeof(Ltotal));
  S->write((uint8_t*)&command,sizeof(command));
  S->write((uint8_t*)url,strlen(url)+1);
  S->write((uint8_t*)&Ldata,sizeof(Ldata));
  S->write((uint8_t*)data,Ldata);

  // garbage (to test)
  const uint8_t g=0x42;
  S->write((uint8_t*)&g,1);
  S->write((uint8_t*)&g,1);

  DBG("arduino sent\n");
  Time::delay(2000);
}
#endif

int k=0;
void application::loop_serial() {
  Time::delay(200);
  return;
  DBG("serial loop\n");
  received::message m;

  debug::turnBuildinLED(true);
  S->wait_for_begin();
    
  bool ok=read_serial(S.get(),reinterpret_cast<uint8_t*>(&m.length),sizeof(m.length));
  DBG("read serial: %d\n",m.length);
  if (!ok) {
    DBG("nothing to read.\n");
    return;
  }
  ok=read_serial(S.get(),m.data,m.length);

  utils::dump(m.data,m.length);
  assert(ok);
  global::queue.push(m);
  debug::turnBuildinLED(false);
  Time::delay(200);
}

class wcallback : public wifi::callback {
  void operator()(uint8_t * data, size_t length) {
    DBG("receiving %d bytes\n",length);
  }
};

void application::loop_wifi() {

  /*
  bool ok=false;
  received::message rx=global::queue.wait(&ok);
  if (!ok) {
    DBG("wifi: empty queue\n");
    return;
  }
  
  received::wifi_command cmd = received::read_wifi_command(rx);

  DBG("wifi read url:%s\n",cmd.url);

  if (cmd.command == 'G')
    W->get(cmd.url,0);

  if (cmd.command == 'P')
    W->post(cmd.url,cmd.data,cmd.Ldata,0);
  */
  assert(W);
  wcallback w;
  const uint8_t data[] = {0xff,0xff,0xff};
  // W->get("http://192.168.178.22:8000/foo/test",&w);
  W->post("http://192.168.178.22:8000/foo/test",data,sizeof(data),&w);
  TRACE();
  Time::delay(5000);
  TRACE();
}


