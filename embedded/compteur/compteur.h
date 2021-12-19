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
    Detection *detection();
    const u8 *data(size_t *L) const;
    static int test();
};
