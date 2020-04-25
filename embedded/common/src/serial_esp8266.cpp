#include "common/serial.h"
#include "freertos/FreeRTOS.h"
#include "driver/uart.h"
#include "common/debug.h"
#include "crc.h"

#define BUF_SIZE 1024
#define PORT UART_NUM_0

// #define DISABLE_SERIAL

serial::serial() {
#ifdef DISABLE_SERIAL
  return;
#endif
  TRACE();
  // For arduino: The default is 8 data bits, no parity, one stop bit.
  uart_config_t uart_config = {
    .baud_rate = 9600,
    .data_bits = UART_DATA_8_BITS,
    .parity    = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
  };
  uart_param_config(PORT, &uart_config);
  uart_driver_install(PORT, BUF_SIZE * 2, BUF_SIZE * 2, 0, NULL);
}

int16_t serial::read(uint8_t *buffer, size_t buffer_size, uint16_t timeout) {
#ifdef DISABLE_SERIAL
  return 0;
#endif
  const int16_t ret=uart_read_bytes(PORT, buffer, buffer_size, timeout / portTICK_RATE_MS);
  if (ret<0) {
    // timeout
    return ret;
  }
  crc::CRC8(&rx_crc8,buffer,ret);
  return ret;
}

size_t serial::write(uint8_t *buffer, size_t buffer_size) {
#ifdef DISABLE_SERIAL
  return 0;
#endif
  size_t ret=uart_write_bytes(PORT, (const char *) buffer, buffer_size);
  crc::CRC8(&tx_crc8,buffer,buffer_size);
  return ret;
}