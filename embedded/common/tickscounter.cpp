#include "tickscounter.h"
#include "clock.h"
#include "debug.h"
#include <limits.h>
#include "eeprom.h"
#include <string.h>
#include "platform.h"

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
  /* There is a subtle reason why we may have
        now == end()
     (and not now > end()). This is because an interrupt may occur during the
     processing of tick(). If less than 1ms has passed until the next tick() 
     processing, we have now=end(). In case of thread-generated interrupts with
     delay(), this can also happen if the call to delay() occurs in a
     call of tick. The next time tick() is entered, no delay() has passed.
     Note that in this case, only one tick is counted, although two (or more)
     occured.
  */
  assert(now>=end());
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

bool tickscounter::load_eeprom() {
  eeprom e;
  const auto Ldst=sizeof(this);
  const auto Lsrc=e.read((uint8_t*)this,Ldst);
  return (Ldst == Lsrc);
}

tickscounter::tickscounter()
  : m_bins{}
{
}

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


void tickscounter::shift_bins(const time_since_epoch delta_seconds) {
  // bin::time max is around 49 days
  bin::time delta = delta_seconds * 1000;

  if (delta<0 || delta>numeric_max<bin::time>())
    return;
  for(int k=0; k<NTICKS; ++k) {
    if (m_bins[k].empty())
      continue;
    m_bins[k].m_start -= delta;
  }
  // m_transmission time does not make sense if have to shift.
  m_transmission_time -= delta;
}

void tickscounter::set_epochtime_at_init(const time_since_epoch T0) {
  if (m_epochtime_at_init!=0) {
    const time_since_epoch delta = T0 - m_epochtime_at_init;
    shift_bins(delta);
  }
  m_epochtime_at_init = T0;
}

tickscounter::tickscounter(const uint8_t *addr) {
  *this = *(tickscounter*)addr;
}

void tickscounter::reset() {
  reset_eeprom();
  for(int k = 0; k<NTICKS; ++k)
    m_bins[k].reset();
  m_transmission_time=0;
} 

int tickscounter::compress_index() {
  bin::duration dmin=0;
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
  DBG("compress");
  clean();
  const int k=compress_index();
  m_bins[k].take(m_bins[k+1]);
  clean();
  assert(is_clean());
}

void move_to_first_empty(const bin (&bins)[NTICKS], int *k) {
  for(; *k<NTICKS && !bins[*k].empty(); ++*k);
}

void move_to_first_non_empty(const bin (&bins)[NTICKS], int *k) {
  for(; *k<NTICKS && bins[*k].empty(); ++*k);
}

bool noise_at_index(const bin (&bins)[NTICKS], int k) {
  const Clock::ms now = Clock::millis_since_start();
  const bin &b=bins[k];
  if (b.empty())
    return false;
  assert(now>=b.end());
  const Clock::ms age = now - b.end();
  const Clock::ms max_age = config::kSecondsUntilAloneTick*1000L;
  if (age>max_age && b.m_count<=config::kMinAloneTicks) {
    // situation where count is too low.
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

bool tickscounter::is_clean() const {
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
  // to allow wiki_work to run at start.
  if (empty())
    return numeric_max<bin::time>();
  const Clock::ms now = Clock::millis_since_start();
  assert(now>=last_tick_time());
  return now - last_tick_time();
}


bool tickscounter::recently_active() {
  constexpr Clock::ms T=config::kRecentlyActiveSeconds*1000L; // 1 min
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

bin tickscounter::getbin(const int &k) const {
  assert(0<=k && k<NTICKS);
  return m_bins[k];
}

uint8_t* tickscounter::getdata(uint16_t * Lout) const {
  m_transmission_time = Clock::millis_since_start();
  DBG("%d\n",m_transmission_time);
  *Lout = sizeof(*this);
  return (uint8_t*)this; 
}

static uint16_t s_total_at_last_save=0;
bool tickscounter::save_eeprom_if_necessary() {
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
  ret+="\"transmit_time\":"+std::to_string(m_transmission_time);
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
#if !defined(ARDUINO) && !defined(ESP8266)
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
  for(; k<config::kMinAloneTicks; ++k) {
    delay(2*one_minute());
    C.tick();
    delay(one_minute());
  }
  return k;
}


#ifndef ARDUINO
#include <fstream>
#endif

int tickscounter::test() {
  tickscounter C;
  assert(C.total()==0);
  int T=0;
  const int K1=NTICKS-2;
  assert(C.total()==T);
  for(int k = 0; k<K1; ++k) {
    delay(one_minute()*2);
    T+=some_real_ticks(C);
    assert(C.total()==T);
  }
  C.print();
  
  for(int k = 0; k<10; ++k) {
    delay(one_minute()*2);
    some_spurious_ticks(C);
    assert(C.total()==T);
  }
 
  const int K2=5;
  for(int k = 0; k<K2; ++k) {
    delay(one_minute()*2);
    T+=some_real_ticks(C);
  }
  assert(T>(K1+K2));
  C.print();
  assert(C.total()==T);

  uint16_t L=0;
  const uint8_t * data = C.getdata(&L);
  DBG("L=%d\n",L);

  tickscounter C2(data);
  assert(C==C2);
#ifndef ARDUINO
  using namespace std;
  std::ofstream file;
  file.open("tickscounter.bin", ios::out | ios::binary);
  file.write((char*)data,L);
#endif

  {
    tickscounter::reset_eeprom();
    if (!C.save_eeprom_if_necessary())
      assert(0);
    C.print();
    tickscounter B;
    B.load_eeprom();
    B.print();
    assert(C==B);
    some_real_ticks(C);
    if (!C.save_eeprom_if_necessary())
      assert(0);
    tickscounter E;
    if (!E.load_eeprom())
      assert(0);
    assert(C==E);
  }
  
  return 0;
}

