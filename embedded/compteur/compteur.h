#pragma once

#include "ticksreader.h"
#include "common/tickscounter.h"

class compteur {
    tickscounter::counter counter;
    TicksReader reader;

  public:
    compteur();
    bool update();
    void print();
    tickscounter::bin::count total();
    TicksReader *ticksReader();
    const u8 *data(size_t *L) const;
    static int test();
};
