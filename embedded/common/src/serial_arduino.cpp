#include "common/serial.h"
#include "common/debug.h"
#include "common/time.h"
#include "crc.h"
#include "common/utils.h"

#define RXCHANNEL Serial
//#define RXCHANNEL SOFT_UART

#define TXCHANNEL Serial
//#define TXCHANNEL SOFT_UART

/*
Board                               Transmit Pin    Receive Pin  Unusable PWM
Arduino Uno, Duemilanove,
LilyPad, Mini (& other ATMEGA328)   9               8            10
Teensy 3.5 / 3.6                    21              20           22
Teensy 3.0 / 3.1 / 3.2              21              20           22
Teensy 2.0                          9               10           (none)
Teensy++ 2.0                        25              4            26, 27

=> TX = 9, RX = 8
 */

namespace delme {
    int available() {
        return RXCHANNEL.available();
    }
}

common::serial::serial() {
    TXCHANNEL.begin(9600);
    RXCHANNEL.begin(9600);
}

void common::serial::reset() {
    TXCHANNEL.end();
    TXCHANNEL.begin(9600);
}

usize common::serial::read(u8 *buffer, usize buffer_size, u16 timeout) {
    // auto soft_uart_timeout = xMax(timeout, u16(100));
    // DBG("serial received %d bytes\r\n", int(buffer_size));
    RXCHANNEL.setTimeout(timeout);
    usize ret = RXCHANNEL.readBytes((char *)buffer, buffer_size);
    crc::CRC8(&rx_crc8, buffer, ret);
    return ret;
}
static int i = 0;
usize common::serial::write(u8 *buffer, usize buffer_size) {
    usize ret = TXCHANNEL.write(buffer, buffer_size);
    TXCHANNEL.flush();
    assert(ret <= buffer_size);
    crc::CRC8(&tx_crc8, buffer, ret);
    return ret;
}
