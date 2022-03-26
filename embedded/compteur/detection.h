#pragma once

#include "intermittentread.h"

class Detection {
    IntermittentRead m_reader;
    bool m_last_value = 1;
    u16 m_last_adc_value[64] = {0};
    common::time::ms m_last_tick_time;
    u8 m_adc_index = 0;
    float xalpha = -1;
    float variance_delta = 0;
    bool tick_worker();
    bool just_ticked();

    u16 m_threshold_min = 0xffff;
    u16 m_threshold_max = 0;
    u16 m_threshold = 10;
    bool adapt_threshold(const u16 &x, const float &xalpha, u16 *threshold);

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
