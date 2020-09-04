#pragma once

#include "common/rusttypes.h"

#pragma pack(push, 4)
struct transient {
    static bool load(transient *t);
    static bool save(transient *);
    static constexpr u8 MAGIC = 0xAB;
    u8 magic = MAGIC;
    u32 n_wakeup = 0;
    float a_fast = 0;
    float a_slow = 0;
};
#pragma pack(pop)
