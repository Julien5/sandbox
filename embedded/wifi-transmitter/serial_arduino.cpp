#include "serial.h"
#include "debug.h"

#include "AltSoftSerial.h"

AltSoftSerial SOFT_UART;

namespace delme {
  int available() {
    return SOFT_UART.available();
  }
}

void serial::serial() {
  DBG("init uart\n");
  SOFT_UART.begin(9600);
}

size_t serial::read(uint8_t *buffer, size_t buffer_size, uint16_t timeout) {
  SOFT_UART.setTimeout(timeout);
  return SOFT_UART.readBytes((char*)buffer,buffer_size);
}

size_t serial::write(uint8_t *buffer, size_t buffer_size) {
  return SOFT_UART.write(buffer,buffer_size);
}
