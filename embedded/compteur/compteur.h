#pragma once

#include "detection.h"
#include "common/tickscounter.h"

class compteur {
    tickscounter::counter m_counter;
    Detection m_detection;
    common::time::ms m_last_ticks[3];

  public:
    compteur();
    bool update();
    void print();
    tickscounter::bin::count total();
    common::time::ms last_tick() const;
    common::time::ms current_period() const;
    common::time::ms previous_period() const;
    Detection *detection();
    bool is_full() const;
    void clear();
    const u8 *data(size_t *L) const;
    static int test();
};
