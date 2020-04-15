#include "common/wifi.h"
#include "common/serial.h"
#include "common/debug.h"

#include <cassert>
#include <string.h>
#define BLOCK_LENGTH 8

namespace wifi {
  std::unique_ptr<serial> S = nullptr;
  wifi::wifi() {
    assert(!S);
    S=std::unique_ptr<serial>(new serial);
  }

  wifi::~wifi() {
  }

  int read_wifi_response(serial *S, callback *r) {
    TRACE();
    bool ok=false;
    while(true) { // FIXME: timeout.
      ok=S->wait_for_begin();
      if (ok)
	break;
    }
    TRACE();
    uint8_t status=0;
    // at this point, the request has been read
    // and is being executed. Network request
    // my take time.
    // timeout is set to 100ms.
    // lets say 20 secs => 200
    uint8_t trials=200;
    ok=false;
    while (!ok && trials--)
      ok=S->read_until(&status,sizeof(status));
    if (!ok) {
      assert(0);
      r->crc(false);
      return 1;
    }
    DBG("received status %d\n",status);
    assert(status==0);
    r->status(status);

    TRACE();
    uint16_t size=0; // FIXME: ntoh
    ok=S->read_until(reinterpret_cast<uint8_t*>(&size),sizeof(size));
    DBG("size:%d\n",int(size));
    if (!ok) {
      assert(0);
      r->crc(false);
      return 2;
    }
    r->data_length(size);

    TRACE();
    size_t nread=0;
    while(nread<size) {
      uint8_t buffer[BLOCK_LENGTH];
      const size_t L=xMin(sizeof(buffer),size-nread);
      ok=S->read_until(buffer,L);
      if (!ok) {
	DBG("nread:%d size:%d\n",nread,size);
	assert(0);
	r->crc(false);
	return 3;
      }
      r->data(buffer,L);
      nread+=L;
    }
    TRACE();
    ok=S->check_end();
    assert(ok);
    r->crc(ok);
    return 0;
  }

  int wifi::get(const char* url, callback * r) {
    const char command = 'G';
    const char * data = nullptr;
    const uint16_t Ldata = 0;
    //                G   http......    0 + Ldata         + data...
    uint16_t Ltotal = 1 + strlen(url) + 1 + sizeof(Ldata) + Ldata;
    
    S->begin();
    TRACE();
    S->write((uint8_t*)&Ltotal,sizeof(Ltotal));
    TRACE();
    S->write((uint8_t*)&command,sizeof(command));
    TRACE();
    S->write((uint8_t*)url,strlen(url)+1);
    TRACE();
    S->write((uint8_t*)&Ldata,sizeof(Ldata));
    TRACE();
    assert(!data);
    TRACE();
    S->end();
    return read_wifi_response(S.get(),r);
  }
  
  int wifi::post(const char* url, const uint8_t * data, const int Ldata, callback * r) {
    const char command = 'P';
    //                G   http......    0 + Ldata         + data...
    const uint16_t Ltotal = 1 + strlen(url) + 1 + sizeof(Ldata) + Ldata;
    TRACE();
    S->begin();
    DBG("Ltotal:%d\n",int(Ltotal));
    S->write((uint8_t*)&Ltotal,sizeof(Ltotal));
    TRACE();
    S->write((uint8_t*)&command,sizeof(command));
    TRACE();
    S->write((uint8_t*)url,strlen(url)+1);
    TRACE();
    S->write((uint8_t*)&Ldata,sizeof(Ldata));
    TRACE();
    S->write((uint8_t*)data,Ldata);
    TRACE();
    S->end();
    TRACE();
    DBG("post done => read response...\n");
    return read_wifi_response(S.get(),r);
  }
}
