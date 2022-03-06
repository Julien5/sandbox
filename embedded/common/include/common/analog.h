#pragma once

#include "common/rusttypes.h"

namespace common {
#ifdef PC
    class analog_read_callback {
      public:
        static void install(analog_read_callback *);
        virtual ~analog_read_callback(){};
        virtual u16 read();
    };
#endif
    class analog {
        u8 m_pin;

      public:
        analog(const u8 &pin = 0);
        virtual ~analog(){};
        virtual u16 read();
    };
}
