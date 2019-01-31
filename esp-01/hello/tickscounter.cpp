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
    return USHRT_MAX;
  if (other.m_start < end())
    return USHRT_MAX;
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
  clean();
  assert(0<=m_compress_index && m_compress_index<(NTICKS-1));
  int k=m_compress_index;
  m_bins[k].take(m_bins[k+1]);
  remove_holes();
  assert(is_clean());
}

constexpr uint8_t clean_threshold = 1;

void move_to_first_empty(const bin bins[], int *k) {
  for(; *k<NTICKS && !bins[*k].empty(); ++*k);
}

void move_to_first_non_empty(const bin bins[], int *k) {
  for(; *k<NTICKS && bins[*k].empty(); ++*k);
}

bool tickscounter::is_clean() const {
  const Clock::mn now = Clock::minutes_since_start();
  for(int k=0; k<NTICKS; ++k) {
    const bin &b=m_bins[k];
    if (b.empty())
      continue;
    assert(now>b.end());
    const Clock::mn age = now - b.end();
    if (age>5 && b.m_count<=clean_threshold) {
      debug("dirt");
      return false;
    }
  }
  
  int k=0;
  move_to_first_empty(m_bins,&k);
  if (k<NTICKS) { 
    for(int l=k; l<NTICKS; ++l)
      if (!m_bins[l].empty()) {
	debug("holes");
	print();
	return false;
      }
  }
  return true;
}

void tickscounter::denoise() {
  const Clock::mn now = Clock::minutes_since_start();
  for(int k=0; k<NTICKS; ++k) {
    bin &b=m_bins[k];
    if (b.empty())
      continue;
    assert(now>b.end());
    const Clock::mn age = now - b.end();
    if (age>5 && b.m_count<=clean_threshold) {
      b.reset();
    }
  }
}

void tickscounter::remove_holes() {
  int k1=0,k2=0;
  while(true) {
    move_to_first_empty(m_bins,&k1);
    if (k1==NTICKS) // bins are full
      break;
    assert(m_bins[k1].empty());
    k2=k1+1;
    move_to_first_non_empty(m_bins,&k2);
    if (k2==NTICKS) // we're done
      break;
    assert(!m_bins[k2].empty());
    m_bins[k1].move(m_bins[k2]);
    k2=0;
  }
}

void tickscounter::clean() {
  bin::count T0=total();
  denoise();
  bin::count T=total();
  assert(T<=T0);
  remove_holes(); 
  update_compress_index();
  // (3) check
  assert(is_clean());
  assert(total()==T);
}

bin::count tickscounter::total() const {
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
  while (!tick_if_possible())
    compress();
}

void tickscounter::print() const {
#ifndef ARDUINO
  for(int k = 0; k<NTICKS; ++k) {
    if (true || !m_bins[k].empty()) {
      printf("%02d: %3d %3d %d\n",k,m_bins[k].m_start,m_bins[k].end(),m_bins[k].m_count);
    }
  }
#endif
}

int some_real_ticks(tickscounter &C) {
  int k=0;
  for(; k<=clean_threshold; ++k)
    C.tick();
  return k;
}

int some_spurious_ticks(tickscounter &C) {
  int k = 0;
  for(; k<clean_threshold; ++k)
    C.tick();
  return k;
}

int tickscounter::test() {
  tickscounter C;
  assert(C.total()==0);
  int T=0;
  const int K1=NTICKS-2;
  for(int k = 0; k<K1; ++k) {
    delay(1000L*60*(K1-k+1));
    T+=some_real_ticks(C);
    assert(C.total()==T);
  }
  
  for(int k = 0; k<10; ++k) {
    delay(1000L*60*(2+k));
    some_spurious_ticks(C);
    assert(C.total()!=T);
  }

  C.print();
  
  const int K2=5;
  for(int k = 0; k<K2; ++k) {
    delay(1000L*60*(K2-k));
    T+=some_real_ticks(C);
    C.print();
    debug("--");
  }
  assert(T>(K1+K2));
  assert(C.total()==T);
  return 0;
}
