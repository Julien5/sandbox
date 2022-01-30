#pragma once

#include "common/rusttypes.h"

namespace common {
    class digital {
      public:
        enum Mode {
            Read,
            Write
        };

      private:
        char m_pin = 0;
        Mode m_mode = Read;

      public:
        digital(char pin, Mode mode = Read);
        ~digital(){};
        bool read();
        void write(bool v);
    };
}
