#include "common/utils.h"
#include "common/rusttypes.h"

#if defined(DEVHOST)
namespace utils {
    std::vector<u8> hex_to_bytes(const std::string &hex) {
        std::vector<u8> bytes;
        for (unsigned int i = 0; i < hex.length(); i += 2) {
            std::string byteString = hex.substr(i, 2);
            u8 byte = (u8)strtol(byteString.c_str(), NULL, 16);
            bytes.push_back(byte);
        }
        return bytes;
    }

    u8 *as_cbytes(std::vector<u8> &bytes, int *L) {
        if (L)
            *L = bytes.size();
        return reinterpret_cast<u8 *>(&bytes[0]);
    }
};
#endif

long long
fixed_atoll(char *s) {
    long long result = 0;
    for (int i = 0; s[i] != '\0'; ++i)
        result = result * 10 + s[i] - '0';
    return result;
}

#if !defined(ARDUINO)
#include <stdio.h>
void utils::dump(const u8 *data_buffer, const size_t length) {
    unsigned char byte;
    unsigned int i, j;
    for (i = 0; i < length; i++) {
        byte = data_buffer[i];
        printf("%02x ", data_buffer[i]); // display byte in hex
        if (((i % 16) == 15) || (i == length - 1)) {
            for (j = 0; j < 15 - (i % 16); j++)
                printf("   ");
            printf("| ");
            for (j = (i - (i % 16)); j <= i; j++) { // display printable bytes from line
                byte = data_buffer[j];
                if ((byte > 31) && (byte < 127)) // outside printable char range
                    printf("%c", byte);
                else
                    printf(".");
            }
            printf("\n"); // end of the dump line (each line 16 bytes)
        }                 // end if
    }                     // end for
}
#else
#include "common/debug.h"
bool ascii(const u8 a) {
    if ('a' <= a && a <= 'z')
        return true;
    if ('A' <= a && a <= 'Z')
        return true;
    if ('0' <= a && a <= '9')
        return true;
    return false;
}
void utils::dump(const u8 *data_buffer, const size_t length) {
    DBG("\r\n");
    for (size_t k = 0; k < length; ++k) {
        if (ascii(data_buffer[k]))
            DBG("%2c ", data_buffer[k]);
        else
            DBG("%02x ", data_buffer[k]);
        if (k % 8 == 0)
            DBG("\r\n");
    }
}
#endif
