#include "application.h"

#include "common/debug.h"
#include "common/time.h"
// #include "common/serial.h"
#include "common/wifi.h"
#include "common/utils.h"

#include "ticksupdater.h"

std::unique_ptr<wifi::wifi> W;
std::unique_ptr<TicksUpdater> ticksUpdater;

void application::setup() {
  debug::init_serial();
  TRACE();
  W=std::unique_ptr<wifi::wifi>(new wifi::wifi);
  TRACE();
  // ticksUpdater=std::unique_ptr<TicksUpdater>(new TicksUpdater);
  TRACE();
}

class wcallback : public wifi::callback {
  size_t missing_bytes=1256;
  void status(uint8_t s) {
    DBG("receiving status %d\r\n",int(s));
    assert(s==0);
  }
  void data_length(uint16_t total_length) {
    DBG("receiving total %d bytes\r\n",int(total_length));
    assert(total_length<10000);
    missing_bytes=total_length;
  }
  void data(uint8_t * data, size_t length) {
    DBG("length %d missing %d\r\n",int(length),int(missing_bytes));
    missing_bytes-=length;
  }
  void crc(bool ok) {
    DBG("receiving crc %d\r\n",int(ok));
    assert(ok);
    /* error, probably to due error on the serial transmission => retry ? */
  }
};

void gather_data() {
  //  ticksUpdater->update();
}

void send_data() {
  wcallback cb;
  auto p=W->get("http://example.com/",&cb);
  //Time::delay(10);
  //uint8_t data[4]={0x01,0x02,0x03,0x04};
  //auto p=W->post("http://postman-echo.com/post",data,sizeof(data),&cb);
  DBG("p:%d\r\n",int(p));
  Time::delay(10);
}

void application::loop()
{
  DBG("<loop>\r\n");
  debug::turnBuildinLED(false);
  // gather_data();
  send_data();
  debug::turnBuildinLED(false);
  DBG("ok\n");
  while(1);
  Time::delay(250);
}
