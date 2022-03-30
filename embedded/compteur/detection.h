#pragma once

#include "intermittentread.h"

class Detection {
    IntermittentRead m_reader;
    bool m_last_value = true;

    float xalpha = -1;
    float variance_delta = 0;
    bool tick_worker();

    float m_delta_power = 0;
    float m_threshold = 100;
    bool adapt_threshold(const float &delta, float *threshold);

  public:
    Detection();
    // returns 1 if the adc just got high.
    bool tick();
#ifdef PC
    static int test();
#endif
};
