#include "message.h"
#include <string.h>
#include "common/debug.h"
#include "common/utils.h"

namespace received {
    message::message() {
        memset(data, 0, sizeof(data) / sizeof(data[0]));
    }

    template <typename T>
    void read(const uint8_t *data, T *ret) {
        uint8_t *ret_addr = reinterpret_cast<uint8_t *>(ret);
        for (size_t i = 0; i < sizeof(T); ++i) {
            ret_addr[i] = data[i];
        }
    }

    wifi_command read_wifi_command(const message &m) {
        wifi_command ret;
        uint8_t *cursor = const_cast<uint8_t *>(m.data);
        read(cursor, &ret.command);
        cursor += sizeof(ret.command);
        ret.url = (char *)cursor;
        cursor += strlen(ret.url) + 1;
        assert(m.length >= (strlen(ret.url) + 2));
        DBG("%d\n", m.length);
        read(cursor, &ret.Ldata);
        assert(ret.Ldata == (m.length - strlen(ret.url) - 4));
        cursor += sizeof(ret.Ldata);
        ret.data = cursor;
        return ret;
    }
}
