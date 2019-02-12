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
  if (empty()) {
    return true;
  }
  Clock::ms now = Clock::millis_since_start();
  assert(now>end());
  if ((now - end()) > 2000)
    return false;
  return true;
}

bool bin::tick() {
  if (!accepts())
    return false;
  const Clock::ms m = Clock::millis_since_start();
  if (empty())
    m_start = m;
  m_duration = m  - m_start;
  m_count++;
  assert(!empty());
  return true;
}

void bin::reset() {
  m_start=m_duration=m_count=0;
}

bool bin::empty() const {
  return m_count == 0;
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
  *this = *(tickscounter*)addr;
}

void tickscounter::reset() {
  for(int k = 0; k<NTICKS; ++k)
    m_bins[k].reset();
  m_tranmission_time=0;
} 

int tickscounter::compress_index() {
  int dmin=0;
  int dmax=0;
  int indx=-1;
  for(int k = 0; (k+1)<NTICKS && !m_bins[k+1].empty(); ++k) {
    bin::duration d = m_bins[k].distance(m_bins[k+1]);
    if (d<dmin || k==0) {
      dmin=d;
      indx = k;
    }
  }
  assert(0<=indx && (indx+1)<NTICKS);
  return indx;
}

void tickscounter::compress() {
  debug("compress");
  clean();
  const int k=compress_index();
  debug(k);
  m_bins[k].take(m_bins[k+1]);
  clean();
  assert(is_clean());
}

constexpr uint8_t clean_threshold = 1;

void move_to_first_empty(const bin bins[], int *k) {
  for(; *k<NTICKS && !bins[*k].empty(); ++*k);
}

void move_to_first_non_empty(const bin bins[], int *k) {
  for(; *k<NTICKS && bins[*k].empty(); ++*k);
}

constexpr int age_for_cleaning_dirt = 5000*60;

bool noise_at_index(const bin bins[], int k) {
  const Clock::ms now = Clock::millis_since_start();
  const bin &b=bins[k];
  if (b.empty())
    return false;
  assert(now>=b.end());
  const Clock::ms age = now - b.end();
  if (age>age_for_cleaning_dirt && b.m_count<=clean_threshold) {
    // situation where count is too low.
    // is it really dirt ?
    // distance to previous and next
    int dprev=USHRT_MAX,dnext=USHRT_MAX;
    if (k>1)
      dprev = bins[k-1].distance(b);
    if ((k+1)<NTICKS && !bins[k+1].empty())
      dnext = b.distance(bins[k+1]);
    if (dprev < 2000 || dnext < 2000)
      return false;
    return true;
  }
  return false;
}

bool tickscounter::is_clean() const {
  const Clock::ms now = Clock::millis_since_start();
  for(int k=0; k<NTICKS; ++k) {
    if (noise_at_index(m_bins,k)) {
      return false;
    }
  }
  
  int k=0;
  move_to_first_empty(m_bins,&k);
  if (k<NTICKS) { 
    for(int l=k; l<NTICKS; ++l)
      if (!m_bins[l].empty()) {
	return false;
      }
  }
  return true;
}


void tickscounter::denoise() {
  const Clock::ms now = Clock::millis_since_start();
  for(int k=0; k<NTICKS; ++k) {
    if (noise_at_index(m_bins,k))
      m_bins[k].reset();
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
  clean();
  int k=0;
  move_to_first_empty(m_bins,&k);
  k--;
  if (k>=0)
    return m_bins[k].end();
  return 0;
}

bin::time tickscounter::age() {
  const Clock::ms now = Clock::millis_since_start();
  assert(now>=last_tick_time());
  return now - last_tick_time();
}


bool tickscounter::recently_active() {
  constexpr Clock::ms T=60*60*1000;
  return age() < T;
}

uint8_t tickscounter::bin_count() const {
  bin::count ret=0;
  for(int k=0; k<NTICKS; ++k)
    if (!m_bins[k].empty())
      ret++;
  return ret;
}

void tickscounter::clean() {
  denoise();
  remove_holes(); 
}

bool tickscounter::empty() const {
  return m_bins[0].empty();
}

bin::count tickscounter::total() {
  clean();
  bin::count ret=0;
  for(int k=0; k<NTICKS; ++k)
    ret+=m_bins[k].m_count;
  return ret;
}

bool tickscounter::tick_if_possible() {
  clean();
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
    if ((k+1)<NTICKS && !m_bins[k+1].empty())
      bins+=",";
    if (m_bins[k+1].empty())
      break;
  }
  ret+="\"bins\":["+bins+"],";
  ret+="\"transmit_time\":"+std::to_string(m_tranmission_time);
  ret+="\n}";
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
    bin::time d=0;
    if ((k+1)<NTICKS)
      d=m_bins[k].distance(m_bins[k+1]);
    printf("%02d: %9d->%-9d [%6d] %3d d=%6d\n",
	   k,m_bins[k].m_start/1000,m_bins[k].end()/1000,m_bins[k].m_duration/1000,
	   m_bins[k].m_count,d);
  }
#endif
}

uint32_t one_minute() {
  return 1000L*60;
}

static int jitter_counter=2;
int jitter(int k) {
  int r=jitter_counter++ % 7;
  if (r%2==0)
    r=-r;
  return r;
}

int some_real_ticks(tickscounter &C) {
  int k=0;
  for(; k<60; ++k) {
    C.tick();
    delay(1200L+jitter(k));
  }
  return k;
}

int some_spurious_ticks(tickscounter &C) {
  int k = 0;
  for(; k<clean_threshold; ++k) {
    delay(2*one_minute());
    C.tick();
    delay(one_minute());
  }
  return k;
}

int tickscounter::test() {
  tickscounter C;
  assert(C.total()==0);
  int T=0;
  const int K1=NTICKS-2;
  for(int k = 0; k<K1; ++k) {
    delay(one_minute()*2);
    T+=some_real_ticks(C);
    assert(C.total()==T);
  }
  C.print();
  
  for(int k = 0; k<10; ++k) {
    delay(one_minute()*2);
    some_spurious_ticks(C);
    assert(C.total()!=T);
  }

  
  const int K2=5;
  for(int k = 0; k<K2; ++k) {
    delay(one_minute()*2);
    T+=some_real_ticks(C);
  }
  assert(T>(K1+K2));
  debug(C.total());
  debug(T);
  C.print();
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

