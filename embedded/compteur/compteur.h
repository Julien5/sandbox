#pragma once

#include "detection.h"
#include "common/tickscounter.h"

class compteur {
    tickscounter::counter counter;
    Detection m_detection;

  public:
    compteur();
    bool update();
    void print();
    tickscounter::bin::count total();
    common::time::ms time_between_last_two_ticks() const;
    float current_rpm();
    Detection *detection();
    bool is_full() const;
    void clear();
    const u8 *data(size_t *L) const;
    static int test();
};
