#include "common/tickscounter.h"
#include "common/clock.h"
#include "common/debug.h"
#include <limits.h>
#include "common/eeprom.h"
#include <string.h>
#include "common/platform.h"
#include "common/time.h"

using namespace tickscounter;

bin::bin(){
  reset();
}

Clock::ms bin::end() const {
  return m_start+m_duration;
}

void bin::tick() {
  const Clock::ms m = Clock::millis_since_start();
  if (empty())
    m_start = m;
  assert(m>=m_start);
  m_duration = m  - m_start;
  m_count++;
  assert(!empty());
}

void bin::reset() {
  m_start=m_duration=m_count=m_pmax=0;
}

bool bin::empty() const {
  return m_count == 0;
}

void bin::take(bin &other) {
  if (other.empty()) {
    return;
  }
  m_count += other.m_count;
  m_pmax = pmax(other);
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
    return 0;
  // DBG("start:%d end:%d start:%d end:%d\n",m_start,end(),other.m_start,other.end());
  if (other.m_start < end()) {
    assert(0);
    return USHRT_MAX;
  }
  return other.m_start - end();
}

bin::duration bin::pmax(const bin &other) const {
  return xMax(xMax(m_pmax,other.m_pmax),distance(other));
}

uint8_t checksum(uint8_t* data, uint16_t L) {
  uint8_t ret=0;
  for(int k=0; k<L; ++k)
    ret+=data[k];
  return ret;
}

class InterruptsDisabler {
public:
  InterruptsDisabler() {
#ifdef ARDUINO
    noInterrupts();
#endif
  }
  ~InterruptsDisabler() {
#ifdef ARDUINO
    interrupts();
#endif
  }
};

bool counter::load_eeprom() {
  eeprom e;
  const auto Ldst=sizeof(this);
  const auto Lsrc=e.read((uint8_t*)this,Ldst);
  return (Ldst == Lsrc);
}

counter::counter(const counter_config c)
  :m_config(c)
{}

template<typename X> X numeric_max() {
  return 0;
}

template<> uint32_t numeric_max() {
  return UINT32_MAX;
}

template<> uint16_t numeric_max() {
  return UINT16_MAX;
}

template<> int32_t numeric_max() {
  return INT32_MAX;
}


void counter::shift_bins(const packed::time_since_epoch delta_seconds) {
  // bin::time max is around 49 days
  Clock::ms delta = delta_seconds * 1000;

  for(int k=0; k<NTICKS; ++k) {
    if (m_packed.m_bins[k].empty())
      continue;
    m_packed.m_bins[k].m_start -= delta;
  }
  // m_transmission time does not make sense if have to shift.
  // do nothing? or reset it?
  m_packed.m_transmission_time = 0;
}

void counter::set_epochtime_at_init(const packed::time_since_epoch T0) {
  if (m_packed.m_epochtime_at_init!=0) {
    const packed::time_since_epoch delta = T0 - m_packed.m_epochtime_at_init;
    shift_bins(delta);
  }
  m_packed.m_epochtime_at_init = T0;
}

void counter::reset() {
  reset_eeprom();
  for(int k = 0; k<NTICKS; ++k)
    m_packed.m_bins[k].reset();
  m_packed.m_transmission_time=0;
} 

int counter::compress_index() {
  bin::duration pmin=0;
  int indx=-1;
  for(int k = 0; (k+1)<NTICKS; ++k) {
    const auto & B1=m_packed.m_bins[k];
    const auto & B2=m_packed.m_bins[k+1];
    const auto p=B1.pmax(B2);
    if (p<pmin || k==0) {
      pmin=p;
      indx = k;
    }
  }
  assert(0<=indx && (indx+1)<NTICKS);
  return indx;
}

void counter::compress() {
  clean();
  const int k=compress_index();
  const auto T0=total();
  m_packed.m_bins[k].take(m_packed.m_bins[k+1]);
  clean();
  assert(total()==T0);
  assert(is_clean());
}

void move_to_first_empty(const bin (&bins)[NTICKS], int *k) {
  for(; *k<NTICKS && !bins[*k].empty(); ++*k);
}

void move_to_first_non_empty(const bin (&bins)[NTICKS], int *k) {
  for(; *k<NTICKS && bins[*k].empty(); ++*k);
}

bool noise_at_index(const bin (&bins)[NTICKS], int k, const int secondsUntilAloneTick, const int minAloneTicks) {
  const Clock::ms now = Clock::millis_since_start();
  const bin &b=bins[k];
  if (b.empty())
    return false;
  assert(now>=b.end());
  const Clock::ms age = now - b.end();
  const Clock::ms max_age = secondsUntilAloneTick*1000L;
  if (age>max_age && b.m_count<minAloneTicks) {
    // situation where count is too low in an old bin.
    // is it really dirt ?
    // distance to previous and next
    Clock::ms dprev=max_age,dnext=max_age;
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

bool counter::is_clean() const {
  for(int k=0; k<NTICKS; ++k) {
    if (noise_at_index(m_packed.m_bins,k,m_config.kSecondsUntilAloneTick,m_config.kMinAloneTicks)) {
      return false;
    }
  }
  
  int k=0;
  move_to_first_empty(m_packed.m_bins,&k);
  if (k<NTICKS) { 
    for(int l=k; l<NTICKS; ++l)
      if (!m_packed.m_bins[l].empty()) {
	return false;
      }
  }
  return true;
}


void counter::denoise() {
  for(int k=0; k<NTICKS; ++k) {
    if (noise_at_index(m_packed.m_bins,k,m_config.kSecondsUntilAloneTick,m_config.kMinAloneTicks)) {
      m_packed.m_bins[k].reset();
    }
  }
}

void counter::remove_holes() {
  int k1=0,k2=0;
  while(true) {
    move_to_first_empty(m_packed.m_bins,&k1);
    if (k1==NTICKS) // bins are full
      break;
    assert(m_packed.m_bins[k1].empty());
    k2=k1+1;
    move_to_first_non_empty(m_packed.m_bins,&k2);
    if (k2==NTICKS) // we're done
      break;
    assert(!m_packed.m_bins[k2].empty());
    m_packed.m_bins[k1].move(m_packed.m_bins[k2]);
    k2=0;
  }
}

Clock::ms counter::last_tick_time() {
  clean();
  int k=0;
  move_to_first_empty(m_packed.m_bins,&k);
  k--;
  if (k>=0)
    return m_packed.m_bins[k].end();
  return 0;
}

Clock::ms counter::age() {
  // to allow wiki_work to run at start.
  if (empty())
    return numeric_max<Clock::ms>();
  const Clock::ms now = Clock::millis_since_start();
  assert(now>=last_tick_time());
  return now - last_tick_time();
}

bool counter::recently_active() {
  const Clock::ms T=m_config.kRecentlyActiveSeconds*1000L; // 1 min
  return age() < T;
}

uint8_t counter::bin_count() const {
  bin::count ret=0;
  for(int k=0; k<NTICKS; ++k)
    if (!m_packed.m_bins[k].empty())
      ret++;
  return ret;
}

void counter::clean() {
  denoise();
  remove_holes(); 
}

bool counter::empty() const {
  return m_packed.m_bins[0].empty();
}

bin::count counter::total() {
  clean();
  bin::count ret=0;
  for(int k=0; k<NTICKS; ++k)
    ret+=m_packed.m_bins[k].m_count;
  return ret;
}

void counter::tick_first_empty_bin() {
  clean();
  for(int k=0; k<NTICKS; ++k) {
    if (m_packed.m_bins[k].empty()) {
      m_packed.m_bins[k].tick();
      return;
    }
  }
}

void counter::tick() {
  tick_first_empty_bin();
  compress();
  assert(m_packed.m_bins[NTICKS-1].empty());
}

bin counter::getbin(const int &k) const {
  assert(0<=k && k<NTICKS);
  return m_packed.m_bins[k];
}

uint8_t* counter::getdata(uint16_t * Lout) const {
  m_packed.m_transmission_time = Clock::millis_since_start();
  *Lout = sizeof(*this);
  return (uint8_t*)this; 
}

static uint16_t s_total_at_last_save=0;
bool counter::save_eeprom_if_necessary() {
  if (empty())
    return false;
  if (total()==s_total_at_last_save)
    return false;
  if (recently_active())
    return false;
  
  uint16_t L=0;
  uint8_t* data=getdata(&L);
  
  eeprom e;
  s_total_at_last_save=total();
  return e.write(data,L) == L;
}

void tickscounter::reset_eeprom() {
  eeprom e;
  const uint8_t zero=0;
  e.write(&zero,1);
  s_total_at_last_save=0;
}

#if !defined(ARDUINO) && !defined(ESP8266)

#include "common/utils.h"
packed tickscounter::fromHex(const std::string &hex) {
  std::vector<uint8_t> bytes=utils::hex_to_bytes(hex);
  return packed(utils::as_cbytes(bytes));
}
std::string packed::json() const {
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
  ret+="\"transmit_time\":"+std::to_string(m_transmission_time);
  ret+="\n}";
  return ret;
}

std::string tickscounter::asJson(const std::string &hex) {
  return fromHex(hex).json();
}
#endif

bool packed::operator==(const packed &other) const {
  for(int k=0;k<NTICKS;++k) {
    if (!(m_bins[k]==m_bins[k]))
      return false;
  }
  return true;
}

void counter::print() const {
#if !defined(ARDUINO) && !defined(ESP8266)
  for(int k = 0; k<NTICKS; ++k) {
    Clock::ms d=0;
    if ((k+1)<NTICKS)
      d=m_packed.m_bins[k].distance(m_packed.m_bins[k+1]);
    printf("%02d: %9d->%-9d [%6d] #=%3d pmax=%6d d=%6d\n",
	   k,
	   m_packed.m_bins[k].m_start/1000,
	   m_packed.m_bins[k].end()/1000,
	   m_packed.m_bins[k].m_duration/1000,
	   m_packed.m_bins[k].m_count,
	   m_packed.m_bins[k].m_pmax,
	   d);
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

int some_real_ticks(counter &C) {
  int k=0;
  for(; k<10; ++k) {
    C.tick();
    Time::delay(1200L+jitter(k));
  }
  return k;
}

int some_spurious_ticks(counter &C, const int minAloneTicks) {
  int k = 0;
  for(; k<minAloneTicks; ++k) {
    Time::delay(2*one_minute());
    C.tick();
    Time::delay(one_minute());
  }
  return k;
}


#ifndef ARDUINO
#include <fstream>
#endif

int tickscounter::test() {
  // disable denoising
  counter_config config;
  config.kMinAloneTicks=0;
  counter C(config);
  {
    C.tick(); Time::delay(1);C.tick();
    Time::delay(10);
    C.tick();Time::delay(1);C.tick();
    Time::delay(7);
    C.tick();Time::delay(1);C.tick();
    C.print();
  }
  C.reset();
  assert(C.total()==0);
  int T=0;
  const int K1=3;
  assert(C.total()==T);
  for(int k = 0; k<K1; ++k) {
    Time::delay(one_minute()*2);
    T+=some_real_ticks(C);
    assert(C.total()==T);
  }
  C.print();
  assert(C.total()==T);
  DBG("T=%d\n",T);
  
  for(int k = 0; k<10; ++k) {
    Time::delay(one_minute()*2);
    some_spurious_ticks(C,C.config().kMinAloneTicks);
    assert(C.total()==T);
  }
 
  const int K2=5;
  for(int k = 0; k<K2; ++k) {
    Time::delay(one_minute()*2);
    T+=some_real_ticks(C);
  }
  assert(T>=(K1+K2));
  C.print();
  assert(C.total()==T);

  uint16_t L=0;
  const uint8_t * data = C.getdata(&L);

  counter C2(data);
  assert(C.get_packed()==C2.get_packed());
#ifndef ARDUINO
  using namespace std;
  std::ofstream file;
  file.open("counter.bin", ios::out | ios::binary);
  file.write((char*)data,L);
#endif

  /*
  {
    tickscounter::reset_eeprom();
    if (!C.save_eeprom_if_necessary())
      assert(0);
    C.print();
    counter B;
    B.load_eeprom();
    B.print();
    assert(C.get_packed()==B.get_packed());
    some_real_ticks(C);
    if (!C.save_eeprom_if_necessary())
      assert(0);
    counter E;
    if (!E.load_eeprom())
      assert(0);
    assert(C.get_packed()==E.get_packed());
  }
  */
  
  return 0;
}

