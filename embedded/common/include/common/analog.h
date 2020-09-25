#pragma once

#include "common/rusttypes.h"

namespace common {
    class analog {
      public:
        virtual ~analog(){};
        virtual u16 read();
    };
}
