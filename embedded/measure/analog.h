#pragma once

#include "common/analog.h"

class analog : public common::analog {
  public:
    u16 read();
};
