#pragma once

#include "common/analog.h"

class analog : public common::analog {
  public:
    virtual u16 read();
};
