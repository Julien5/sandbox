#include "common/serial.h"
#include "common/debug.h"
#include "common/utils.h"
#include <vector>
#include <cstring>

#include <mutex>
#include "crc.h"

namespace {
  std::vector<uint8_t> data;
  std::mutex mtx;
}

serial::serial() {
}

size_t serial::read(uint8_t *buffer, size_t buffer_size, uint16_t timeout) {
  const std::lock_guard<std::mutex> lock(mtx);
  const size_t Lread = xMin(buffer_size,data.size());
  memcpy(buffer,data.data(),Lread);
  data.erase(data.begin(),data.begin()+Lread);
  crc::CRC8(&rx_crc8,buffer,Lread);
  if (Lread > 0) {
    DBG("read %d from %d bytes\n",Lread,data.size());
    utils::dump(buffer,Lread);
  }
  return Lread;
}

size_t serial::write(uint8_t *buffer, size_t buffer_size) {
  const std::lock_guard<std::mutex> lock(mtx);
  for(size_t i = 0; i<buffer_size; ++i)
    data.push_back(buffer[i]);
  crc::CRC8(&tx_crc8,buffer,buffer_size);
  DBG("write %d to %d bytes\n",buffer_size,data.size());
  utils::dump(buffer,buffer_size);
  return buffer_size;
}
