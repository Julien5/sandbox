#include "tickscounter.h"
#include "clock.h"

#include <limits.h>

template<typename T>
T abs(T x) {
  if (x>0)
    return x;
  return -x;
}

bin::bin(){}

bin::time bin::end() const {
  return m_start+m_duration;
}

bool bin::accepts() const {
  Clock::mn min = Clock::minutes_this_month();
  if (abs(min - end()) > 1)
    return false;
  return true;
}

bool bin::tick() {
  if (!accepts())
    return false;
  m_duration = Clock::minutes_this_month() - m_start;
  m_count++;
}

void bin::reset() {
  m_duration=m_count=0;
}

bool bin::empty() const {
  return m_duration == 0 && m_count == 0;
}

void bin::take(bin &other) {
  m_count += other.m_count;
  m_duration = other.end() - m_start;
  other.reset();
}

bin::duration bin::distance(const bin &other) {
  if (empty() || other.empty())
    return INT_MAX;
  if (other.m_start < end())
    return INT_MAX;
  return other.m_start - end();
}

tickscounter::tickscounter()
  : m_bins{}
{}

void tickscounter::update_compress_index(int startindex) {
  if (startindex == 1)
    m_dmin=0;
  for(int k = startindex; k<(NTICKS-1) && !m_bins[k+1].empty(); ++k) {
    bin::duration d = m_bins[k-1].distance(m_bins[k]);
    if (d<m_dmin || m_dmin==0) {
      m_dmin=d;
      m_compress_index = k-1;
    }
  }
}

void tickscounter::compress() {
  m_bins[m_compress_index].take(m_bins[m_compress_index+1]);
  update_compress_index();
  
}

bool tickscounter::tick_if_possible() {
  for(int k=0; k<NTICKS; ++k)
    if (m_bins[k].tick()) {
      update_compress_index(k);
      return true;
    }
  return false;
}

void tickscounter::tick() {
  while (!tick_if_possible())
    compress();
}

int tickscounter::test() {
}
