#include "common/serial.h"
#include "common/debug.h"
#include "common/time.h"
#include "crc.h"
#include "common/utils.h"
#include "AltSoftSerial.h"

AltSoftSerial SOFT_UART;

#define RXCHANNEL Serial
//#define RXCHANNEL SOFT_UART

/*
Board	Transmit Pin	Receive Pin	Unusable PWM
Teensy 3.5 / 3.6	21	20	22
Teensy 3.0 / 3.1 / 3.2	21	20	22
Teensy 2.0	9	10	(none)
Teensy++ 2.0	25	4	26, 27
Arduino Uno, Duemilanove,
LilyPad, Mini (& other ATMEGA328)	9	8	10

=> TX = 9, RX = 8
 */

namespace delme {
    int available() {
        return RXCHANNEL.available();
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

usize common::serial::read(u8 *buffer, usize buffer_size, u16 timeout) {
    auto soft_uart_timeout = xMax(timeout, u16(100));
    //#define CHANNEL SOFT_UART
    //DBG("(available:%d\r\n", int(RXCHANNEL.available()));
    RXCHANNEL.setTimeout(timeout);
    usize ret = RXCHANNEL.readBytes((char *)buffer, buffer_size);
    if (ret > 0)
        utils::dump(buffer, buffer_size);
    //DBG("ret:%d \r\n", int(ret));
    //common::time::delay(100);
    crc::CRC8(&rx_crc8, buffer, ret);
    return ret;
}

usize common::serial::write(u8 *buffer, usize buffer_size) {
    usize ret = SOFT_UART.write(buffer, buffer_size);
    SOFT_UART.flush();
    assert(ret <= buffer_size);
    crc::CRC8(&tx_crc8, buffer, ret);
    return ret;
}
