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
      public:
        virtual ~analog(){};
        virtual u16 read();
    };
}
