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
    u32 t0 = common::Time::since_reset();
    while ((addr - addr0) != int(L)) {
        const auto Lwanted = L - (addr - addr0);
        const u16 timeout_local = 100;
        const auto Lread = read(addr, Lwanted, timeout_local);
        const auto elapsed = common::Time::since_reset() - t0;
        if (timeout > 0 && elapsed > timeout)
            return false;
        if (Lread < 0)
            return false;
        addr += Lread;
    }
    return true;
}

bool serial::wait_for_begin() {
    while (true) {
        rx_crc8 = 0x00;
        u8 begin = 0;
        bool ok = read_until(&begin, sizeof(begin));
        if (!ok)
            continue;
        if (begin == kBegin)
            break;
        // DBG("waiting for begin (received 0x%02x)\n",begin);
    }
    return true;
}

bool serial::check_end() {
    // reading modifies rx_crc8
    // so we must save the last value to check it against the transmitted crc.
    // alternatively, we could check that the crc is null
    u8 crc8_received = 0;
    auto saved_rx_crc8 = rx_crc8;
    while (true) {
        bool ok = read_until(reinterpret_cast<u8 *>(&crc8_received), sizeof(crc8_received));
        if (ok)
            break;
    }
    if (crc8_received != saved_rx_crc8)
        DBG("CRC: 0x%02x != 0x%02x\r\n", crc8_received, saved_rx_crc8);
    auto match = crc8_received == saved_rx_crc8;
    if (!match) {
        assert(0);
    }
    return match;
}
