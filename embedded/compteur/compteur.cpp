#include "compteur.h"
#include "common/debug.h"
#include "common/time.h"

tickscounter::counter_config
config() {
    tickscounter::counter_config ret;
    ret.kMinAloneTicks = 0;
    ret.kSecondsUntilAloneTick = 3;
    return ret;
}

compteur::compteur()
    : counter(config()) {
    debug::address_range("compteur:", this, sizeof(*this));
}

double
kW(const u32 ticks, const Clock::ms d) {
    if (d == 0)
        return 0;
    const double hours = double(d) * 0.001 / 3600.0f;
    return (double(ticks) / hours) / 75.0f;
}

void print_bin(const tickscounter::bin &b) {
    // DBG("start:%7d duration:%7d count:%3d\n",b.m_start, b.m_duration,b.m_count);
}

bool compteur::update() {
    /*
  const auto t = Time::since_reset();
  if (t%10000==0 && counter.bin_count()>2) {
    const auto bin0=counter.getbin(0);
    const auto bin2=counter.getbin(counter.bin_count()-2);
    const auto bin1=counter.getbin(counter.bin_count()-1);
    const auto time_total=bin1.end()-bin0.m_start;
    DBG("seconds:%3d total:%2d count:%2d power:%2.3fkW  current-power:%2.3fkW\n",
	t/1000,counter.total(),counter.bin_count(),
	kW(counter.total()-1,time_total),
	kW(bin1.m_count+bin2.m_count-1,bin1.end()-bin2.m_start));
    
  }
  */
    if (reader.take()) {
        counter.tick();
        return true;
    }
    return false;
}

void compteur::print() {
    // DBG("time:%4d sec total:%d\n",int(Time::since_reset()/1000),int(counter.total()));
}

const u8 *
compteur::data(size_t *L) const {
    return reinterpret_cast<const u8 *>(counter.get_packed(L));
}

tickscounter::bin::count
compteur::total() {
    return counter.total();
}

int compteur::test() {
    compteur U;
    while (true) {
        if (U.update())
            U.print();
        Time::delay(200);
    }
}
