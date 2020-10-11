#include "common/serial.h"
#include "common/debug.h"
#include "common/time.h"
#include "crc.h"

#include "AltSoftSerial.h"

AltSoftSerial SOFT_UART;

namespace delme {
    int available() {
        return SOFT_UART.available();
    }
}

common::serial::serial() {
    debug::address_range("serial:", this, sizeof(*this));
    SOFT_UART.begin(9600);
}

void common::serial::reset() {
    SOFT_UART.end();
    SOFT_UART.begin(9600);
}

i16 common::serial::read(u8 *buffer, size_t buffer_size, u16 timeout) {
    auto soft_uart_timeout = xMax(timeout, u16(100));
    SOFT_UART.setTimeout(soft_uart_timeout);
    size_t ret = SOFT_UART.readBytes((char *)buffer, buffer_size);
    common::time::delay(100);
    crc::CRC8(&rx_crc8, buffer, ret);
    return ret;
}

size_t common::serial::write(u8 *buffer, size_t buffer_size) {
    size_t ret = SOFT_UART.write(buffer, buffer_size);
    SOFT_UART.flush();
    assert(ret <= buffer_size);
    crc::CRC8(&tx_crc8, buffer, ret);
    return ret;
}
