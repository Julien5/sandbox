#pragma once

#include "common/rusttypes.h"
#include <stdlib.h>

namespace wifi {
    class callback {
      public:
        virtual ~callback(){};
        virtual void status(u8 s) = 0;
        virtual void data_length(u16 total_length) = 0;
        virtual void data(u8 *data, size_t length) = 0;
        virtual void crc(bool ok) = 0;
    };

    class wifi {
      public:
        wifi();
        ~wifi();

        int get(const char *req, callback *r);
        int post(const char *req, const u8 *data, const u16 Ldata, callback *r);
    };
}
