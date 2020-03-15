#include "serial.h"
#include "common/debug.h"
#include <vector>
#include <cstring>

#include <mutex>

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
  return Lread;
}

size_t serial::write(uint8_t *buffer, size_t buffer_size) {
  const std::lock_guard<std::mutex> lock(mtx);
  for(size_t i = 0; i<buffer_size; ++i)
    data.push_back(buffer[i]);
  return buffer_size;
}
