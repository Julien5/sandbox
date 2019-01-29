#include "tickscounter.h"
#include "clock.h"
#include "debug.h"
#include <limits.h>

#ifndef abs
#define abs(x) ((x)>0?(x):-(x))
#endif

bin::bin(){
  reset();
}

bin::time bin::end() const {
  return m_start+m_duration;
}

bool bin::accepts() const {
  if (empty())
    return true;
  Clock::mn min = Clock::minutes_since_start();
  if (abs(min - end()) > 1)
    return false;
  return true;
}

bool bin::tick() {
  if (!accepts())
    return false;
  Clock::mn min = Clock::minutes_since_start();
  if (empty())
    m_start = min;
  m_duration = min  - m_start;
  m_count++;
  return true;
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

void bin::move(bin &other) {
  *this = other;
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

void tickscounter::update_compress_index() {
  m_dmin=0;
  for(int k = 0; k<(NTICKS-1) && !m_bins[k+1].empty(); ++k) {
    bin::duration d = m_bins[k-1].distance(m_bins[k]);
    if (d<m_dmin || m_dmin==0 || m_compress_index < 0) {
      m_dmin=d;
      m_compress_index = k-1;
    }
  }
}

void tickscounter::compress() {
  debug("compress");
  assert(0<=m_compress_index && m_compress_index<(NTICKS-1));
  bin::count T=total();
  debug(m_compress_index);
  int k=m_compress_index;
  m_bins[k].take(m_bins[k+1]);
  assert(m_bins[k+1].empty());
  k++;
  while((k+1)<NTICKS) {
    m_bins[k].move(m_bins[k+1]);
    k++;
  }
  update_compress_index();
  assert(total()==T);
}

bin::count tickscounter::total() {
  bin::count ret=0;
  for(int k=0; k<NTICKS; ++k)
    ret+=m_bins[k].m_count;
  return ret;
}

bool tickscounter::tick_if_possible() {
  for(int k=0; k<NTICKS; ++k) {
    if (m_bins[k].tick()) {
      update_compress_index();
      return true;
    }
  }
  return false;
}

void tickscounter::tick() {
  bin::count T=total();
  while (!tick_if_possible())
    compress();
  assert(total()==T+1);
}

void tickscounter::print() {
#ifndef ARDUINO
  for(int k = 0; k<NTICKS; ++k) {
    if (!m_bins[k].empty()) {
      printf("%02d: %3d %3d %d\n",k,m_bins[k].m_start,m_bins[k].end(),m_bins[k].m_count);
    }
  }
#endif
}

int tickscounter::test() {
  tickscounter C;
  assert(C.total()==0);
  const int K1=NTICKS-2;
  for(int k = 0; k<K1; ++k) {
    delay(1000L*60*(K1-k+1));
    C.tick();
    assert(C.total()==k+1);
  }
  const int K2=5;
  for(int k = 0; k<K2; ++k) {
    delay(1000L*60*(K2-k));
    C.tick();
    C.print();
    debug("--");
  }
  assert(C.total()==(K1+K2));
  return 0;
}
