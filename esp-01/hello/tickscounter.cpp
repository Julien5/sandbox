#include "tickscounter.h"
#include "clock.h"
#include "debug.h"
#include <limits.h>

#ifndef ARDUINO
#include <fstream>
#endif

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
  const Clock::mn min = Clock::minutes_since_start();
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

bool bin::operator==(const bin &other) const  {
  if (m_start != other.m_start)
    return false;
  if (m_count != other.m_count)
    return false;
  if (m_duration != other.m_duration)
    return false;
  return true;
}

bin::duration bin::distance(const bin &other) const {
  if (empty() || other.empty())
    return USHRT_MAX;
  if (other.m_start < end())
    return USHRT_MAX;
  return other.m_start - end();
}

tickscounter::tickscounter()
  : m_bins{}
{}

tickscounter::tickscounter(const uint8_t *addr) {
  const int this_version = this->version;
  *this = *(tickscounter*)addr;
  if (this->version != this_version)
    debug("versioning problem!");
}

int tickscounter::compress_index() {
  int dmin=0;
  int indx=-1;
  for(int k = 0; (k+1)<NTICKS && !m_bins[k+1].empty(); ++k) {
    bin::duration d = m_bins[k].distance(m_bins[k+1]);
    if (d<dmin || k==0) {
      dmin=d;
      indx = k;
    }
  }
  assert(0<=indx && indx<(NTICKS-1));
  return indx;
}

void tickscounter::compress() {
  debug("compress");
  clean();
  const int k=compress_index();
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
    assert(now>=b.end());
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

bin::time tickscounter::last_tick_time() {
  denoise();
  int k=0;
  move_to_first_empty(m_bins,&k);
  k--;
  if (k>=0)
    return m_bins[k].end();
  return 0;
}

bool tickscounter::recently_active() {
  return Clock::minutes_since_start() - last_tick_time() < 60;
}

uint8_t tickscounter::bin_count() const {
  bin::count ret=0;
  for(int k=0; k<NTICKS; ++k)
    if (!m_bins[k].empty())
      ret++;
  return ret;
}

void tickscounter::clean() {
  bin::count T0=total();
  denoise();
  bin::count T=total();
  assert(T<=T0);
  remove_holes(); 
  // (3) check
  assert(is_clean());
  assert(total()==T);
}

bool tickscounter::empty() const {
  return m_bins[0].empty();
}

bin::count tickscounter::total() {
  denoise();
  bin::count ret=0;
  for(int k=0; k<NTICKS; ++k)
    ret+=m_bins[k].m_count;
  return ret;
}

bool tickscounter::tick_if_possible() {
  for(int k=0; k<NTICKS; ++k) {
    if (m_bins[k].tick()) {
      return true;
    }
  }
  return false;
}

void tickscounter::tick() {
  while (!tick_if_possible())
    compress();
}

uint8_t* tickscounter::getdata(int * Lout) const {
  m_tranmission_time = Clock::minutes_since_start();
  *Lout = sizeof(*this);
  return (uint8_t*)this; 
}

#ifndef ARDUINO
#include "utils.h"
tickscounter tickscounter::fromHex(const std::string &hex) {
  std::vector<uint8_t> bytes=utils::hex_to_bytes(hex);
  return tickscounter(utils::as_cbytes(bytes));
}
std::string tickscounter::json() const {
  std::string ret;
  ret+="{\n";
  std::string bins;
  for(int k=0; k<NTICKS;++k) {
    bins+=std::string("{")
      + "\"start\":"+std::to_string(m_bins[k].m_start)
      +",\"count\":"+std::to_string(m_bins[k].m_count)
      +",\"duration\":"+std::to_string(m_bins[k].m_duration)
      +"}";
    if ((k+1)<NTICKS || !m_bins[k+1].empty())
      bins+=",";
    if (m_bins[k+1].empty())
      break;
  }
  ret+="\"bins\":["+bins+"]\n}";
  return ret;
}

std::string tickscounter::asJson(const std::string &hex) {
  return fromHex(hex).json();
}
#endif

bool tickscounter::operator==(const tickscounter &other) const {
  for(int k=0;k<NTICKS;++k) {
    if (!(m_bins[k]==other.m_bins[k]))
      return false;
  }
  return true;
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

uint32_t one_minute() {
  return 1000L*60;
}

int some_real_ticks(tickscounter &C) {
  int k=0;
  for(; k<=60; ++k) {
    C.tick();
    delay(1200L);
  }
  return k;
}

int some_spurious_ticks(tickscounter &C) {
  int k = 0;
  for(; k<clean_threshold; ++k) {
    delay(one_minute()*2);
    C.tick();
  }
  return k;
}

int tickscounter::test() {
  tickscounter C;
  assert(C.total()==0);
  int T=0;
  const int K1=NTICKS-2;
  for(int k = 0; k<K1; ++k) {
    delay(one_minute()*2*(K1-k+1));
    T+=some_real_ticks(C);
    assert(C.total()==T);
  }
  
  for(int k = 0; k<10; ++k) {
    delay(one_minute()*2*(2+k));
    some_spurious_ticks(C);
    assert(C.total()!=T);
  }

  C.print();
  
  const int K2=5;
  for(int k = 0; k<K2; ++k) {
    delay(one_minute()*(K2-k));
    T+=some_real_ticks(C);
    C.print();
    debug("--");
  }
  assert(T>(K1+K2));
  assert(C.total()==T);

  int L=0;
  const uint8_t * data = C.getdata(&L);
  debug(L);

  tickscounter C2(data);
  assert(C==C2);
#ifndef ARDUINO
  using namespace std;
  std::ofstream file;
  file.open("tickscounter.bin", ios::out | ios::binary);
  file.write((char*)data,L);
#endif
  return 0;
}

