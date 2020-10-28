#pragma once

#include "common/rusttypes.h"

namespace common {
    class digital {
      public:
        virtual ~digital(){};
        virtual bool read();
        virtual void write(bool v);
    };
}
