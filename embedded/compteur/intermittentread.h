#pragma once
#include "adcfile.h"
#include "common/time.h"
#include "common/platform.h"
#include "common/analog.h"

class IntermittentRead {
  public:
    static const int T0 = 3;
    static const int T = 5;

  private:
    size_t k = 0;
    common::time::us last_measure_time = common::time::us(0);
    int A[T] = {0};
    std::unique_ptr<common::analog> m_analog;
    int value(const size_t k);

  public:
    IntermittentRead();
    common::time::us micros_since_last_measure() const;
    bool tick(u16 *value);
    bool done() const;
    bool old() const;
    void reset();
    float average() const;
};
