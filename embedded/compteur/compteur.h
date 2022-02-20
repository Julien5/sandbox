#pragma once

#include "detection.h"
#include "common/tickscounter.h"

class compteur {
    tickscounter::counter counter;
    Detection m_detection;
    common::time::ms m_last_ticks[3];

  public:
    compteur();
    bool update();
    void print();
    tickscounter::bin::count total();
    common::time::ms time_between_last_two_ticks() const;
    float current_rpm();
    float delta();
    Detection *detection();
    bool is_full() const;
    void clear();
    const u8 *data(size_t *L) const;
    static int test();
};
