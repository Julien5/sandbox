#include "common/serial.h"
#include "common/debug.h"
#include "common/utils.h"
#include "common/time.h"

const u8 kBegin = 0xFF;

bool serial::begin() {
    tx_crc8 = 0x00;
    auto n = write((u8 *)&kBegin, 1);
    return n == 1;
}

bool serial::end() {
    auto n = write((u8 *)&tx_crc8, 1);
    return n == 1;
}

bool serial::read_until(u8 *addr, const size_t &L, const u16 &timeout) {
    const auto addr0 = addr;
    while ((addr - addr0) != int(L)) {
        const auto Lwanted = L - (addr - addr0);
        const auto Lread = read(addr, Lwanted, timeout);
        if (Lread < 0) // timeout
            return false;
        addr += Lread;
    }
    return true;
}

bool serial::wait_for_begin(const u16 &timeout) {
    rx_crc8 = 0x00;
    u8 begin = 0;
    bool ok = read_until(&begin, sizeof(begin), timeout);
    if (!ok)
        return false;
    return (begin == kBegin);
}

bool serial::check_end() {
    // reading modifies rx_crc8
    // so we must save the last value to check it against the transmitted crc.
    // alternatively, we could check that the crc is null
    u8 crc8_received = 0;
    auto saved_rx_crc8 = rx_crc8;
    while (true) {
        bool ok = read_until(reinterpret_cast<u8 *>(&crc8_received), sizeof(crc8_received), 10);
        if (ok)
            break;
    }
    if (crc8_received != saved_rx_crc8)
        DBG("CRC: 0x%02x != 0x%02x\r\n", crc8_received, saved_rx_crc8);
    auto match = crc8_received == saved_rx_crc8;
    if (!match) {
        // assert(0);
    }
    return match;
}
