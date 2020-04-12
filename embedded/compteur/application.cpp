#include "application.h"

#include "common/debug.h"
#include "common/time.h"
#include "common/serial.h"
#include "common/wifi.h"

std::unique_ptr<wifi::wifi> W;

void application::setup() {
  W=std::unique_ptr<wifi::wifi>(new wifi::wifi);
}

class wcallback : public wifi::callback {
  void status(uint8_t s) {
    DBG("receiving status %d \n",int(s));
  }
  void data_length(uint16_t total_length) {
    DBG("receiving total %d bytes\n",total_length);
  }
  void data(uint8_t * data, size_t length) {
    DBG("receiving %d bytes\n",length);
  }
  void crc(bool ok) {
    DBG("receiving crc %d \n",int(ok));
  }
};

void application::loop()
{
  DBG("ok\n");
  uint8_t data[4]={0};
  wcallback cb;
  W->post("http://foo.bar/xx",data,sizeof(data),&cb);
  Time::delay(10);
}
