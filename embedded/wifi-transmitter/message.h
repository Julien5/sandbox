#pragma once

#include <vector>
#include "common/rusttypes.h"

namespace received {
    struct message {
        u8 data[2048];
        u16 length = {0};
        message();
    };

    struct wifi_command {
        char command = 0;
        char *url = {0};
        u16 Ldata = 0;
        u8 *data = {0};
    };

    wifi_command read_wifi_command(const message &m);

    void test();
};
