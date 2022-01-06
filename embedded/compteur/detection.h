#pragma once

#include "intermittentread.h"
#include "histogram.h"

class Detection {
    IntermittentRead m_reader;
    bool m_last_value = 1;
    u16 m_last_adc_value[64] = {0};
    u8 m_adc_index = 0;

  public:
    Detection();
    // returns 1 if the adc just got high.
    bool tick();
    const u8 *histogram_data(usize *L) const;
    const u8 *adc_data(usize *L = 0) const;
#ifdef PC
    static int test();
#endif
};
