#include "crc.h"
#include "common/debug.h"
// http://www.devcoons.com/crc8/
void crc::CRC8(uint8_t *init, const uint8_t *const data, size_t length) {
    uint8_t sum;
    for (size_t i = 0; i < length; i++) {
        uint8_t extract = data[i];
        for (uint8_t tempI = 8; tempI; tempI--) {
            sum = (*init ^ extract) & 0x01;
            *init >>= 1;
            if (sum)
                *init ^= 0x8C;
            extract >>= 1;
        }
    }
}

int crc::test() {
    uint8_t data[16];
    for (size_t k = 0; k < sizeof(data); ++k) {
        data[k] = k & 0xFF;
    }
    uint8_t orig = 0x00;
    CRC8(&orig, data, sizeof(data));
    printf("orig: 0x%02x\n", orig);
    for (size_t j = 0; j < sizeof(data); ++j) {
        for (size_t k = 0; k < sizeof(data); ++k) {
            data[k] = k & 0xFF;
        }
        // inverse bit at position j modulo 8.
        const uint8_t mini = 1 << (j % 8);
        const auto a = data[j];
        data[j] ^= mini;
        const auto b = data[j];
        assert(a != b);
        uint8_t c = 0xFF;
        CRC8(&c, data, sizeof(data));
        DBG("j=%02d, mini=0x%02x, a=0x%02x, b=0x%02x => crc: 0x%02x vs 0x%02x, OK=%d\n", j, mini, a, b, c, orig, int(c != orig));
        if (c == orig)
            return 1;
    }
    return 0;
}
