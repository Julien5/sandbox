#include "application.h"

#include "common/debug.h"
#include "common/time.h"
#include "common/serial.h"
#include "common/wifi.h"
#include "common/utils.h"

std::unique_ptr<wifi::wifi> W;

void application::setup() {
  W=std::unique_ptr<wifi::wifi>(new wifi::wifi);
}

class wcallback : public wifi::callback {
  void status(uint8_t s) {
    DBG("receiving status %d \n",int(s));
    assert(s==0);
  }
  void data_length(uint16_t total_length) {
    DBG("receiving total %d bytes\n",total_length);
  }
  void data(uint8_t * data, size_t length) {
    DBG("receiving %d bytes\n",length);
    utils::dump(data,length);
  }
  void crc(bool ok) {
    DBG("receiving crc %d \n",int(ok));
    assert(ok);
  }
};

void application::loop()
{
  DBG("\n\n\n\n -- loop -- \n\n\n\n");
  wcallback cb;
  //W->get("http://example.com/",&cb);
  Time::delay(10);
  uint8_t data[4]={0x01,0x02,0x03,0x04};
  W->post("http://postman-echo.com/post",data,sizeof(data),&cb);
  Time::delay(10);
}
