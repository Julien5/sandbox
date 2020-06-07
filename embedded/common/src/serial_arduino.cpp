#include "common/serial.h"
#include "common/debug.h"
#include "crc.h"

#include "AltSoftSerial.h"

AltSoftSerial SOFT_UART;

namespace delme {
  int available() {
    return SOFT_UART.available();
  }
}

serial::serial() {
  SOFT_UART.begin(9600);
}

int16_t serial::read(uint8_t *buffer, size_t buffer_size, uint16_t timeout) {
  SOFT_UART.setTimeout(timeout);
  size_t ret=SOFT_UART.readBytes((char*)buffer,buffer_size);
  crc::CRC8(&rx_crc8,buffer,ret);
  return ret;
}

size_t serial::write(uint8_t *buffer, size_t buffer_size) {
  size_t ret=SOFT_UART.write(buffer,buffer_size);
  assert(ret<=buffer_size);
  crc::CRC8(&tx_crc8,buffer,ret);
  return ret;
}
