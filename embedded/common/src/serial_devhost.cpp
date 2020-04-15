#include "common/serial.h"
#include "common/debug.h"
#include "common/utils.h"
#include <vector>
#include <cstring>
#include <condition_variable>	

#include <mutex>
#include "crc.h"

struct serial_buffer {
  std::vector<uint8_t> data;
  std::mutex mtx;
  std::condition_variable cond_var;
public:
  int16_t read(uint8_t *buffer, size_t buffer_size, uint16_t timeout) {
    std::unique_lock<std::mutex> lock(mtx);
    if (data.empty()) {
      DBG("serial read waiting.\n");
      cond_var.wait_for(lock,std::chrono::milliseconds(timeout));
    }
    if (data.empty())
      return -1;
    const size_t Lread = xMin(buffer_size,data.size());
    memcpy(buffer,data.data(),Lread);
    data.erase(data.begin(),data.begin()+Lread);
    return Lread;
  }
  size_t write(uint8_t *buffer, size_t buffer_size) {
    const std::unique_lock<std::mutex> lock(mtx);
    for(size_t i = 0; i<buffer_size; ++i)
      data.push_back(buffer[i]);
    cond_var.notify_one();
    return buffer_size;
  }
};

namespace {
  serial_buffer s_rxbuffer;
  serial_buffer s_txbuffer;
}

#include <map>

struct G {
  std::mutex mtx;
  std::map<serial*,int> m;
  static int counter;
  int get(serial *s) {
    std::unique_lock<std::mutex> lock(mtx);
    auto ret=m[s];
    return ret;
  }
  void record(serial *s) {
    std::unique_lock<std::mutex> lock(mtx);
    DBG("counter:%d\n",counter);
    m[s]=counter++;
  }
};

namespace {
  G s_map;
}

int G::counter=0;

serial::serial() {
  s_map.record(this);
}

int16_t serial::read(uint8_t *buffer, size_t buffer_size, uint16_t timeout) {
  auto & buf=s_map.get(this)==0 ? s_rxbuffer : s_txbuffer;
  DBG("read from %p\n",&buf);
  const auto Lread=buf.read(buffer,buffer_size,timeout);
  if (Lread<0) {
    DBG("timeout %d\n",timeout);
    return Lread;
  }
  
  crc::CRC8(&rx_crc8,buffer,Lread);
  
  DBG("read %d bytes (from %d) => now %d bytes\n",Lread,buffer_size,buf.data.size());
  if (Lread>0)
    utils::dump(buffer,Lread);
  
  return Lread;
}

size_t serial::write(uint8_t *buffer, size_t buffer_size) {
  auto & buf=s_map.get(this)==0 ? s_txbuffer : s_rxbuffer;
  DBG("write to %p\n",&buf);
  buf.write(buffer,buffer_size);
  crc::CRC8(&tx_crc8,buffer,buffer_size);
  DBG("write %d bytes => now %d bytes\n",buffer_size,buf.data.size());
  utils::dump(buffer,buffer_size);
  return buffer_size;
}
