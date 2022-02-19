#pragma once
#include "common/rusttypes.h"
#include "common/wifi.h"

class httpsender {
    wifi::wifi m_wifi;

  public:
    httpsender();
    ~httpsender();
    bool post_tickcounter(const u8 *data, const usize &length);
    bool get_epoch(u64 *epoch);
};
