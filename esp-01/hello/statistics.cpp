#include "statistics.h"
#include "debug.h"
#include "eeprom.h"
#include "freememory.h"
#include <cstring>

// hi

#ifdef ARDUINO
#include "Arduino.h"
#else
long long millis() {
  assert(0);
  return 0;
}
void delay(long long d) {
  assert(0);
}
#endif

using namespace types;

namespace {
  bool artificial = false;
  using ms = types::milli;
  ms t=0;
  ms now() {
    if (!artificial)
      return millis();
    return t;
  }
  void sleep(ms _delay) {
    if (!artificial)
      return delay(_delay);
    t+=_delay;
  }

  ms t0=0;
  void set_start_time(const ms &m) {
    t0=m;
  }

  milli get_milli(const ms &m) {
    return m - t0;
  }
  
  minute get_minute(const ms &m) {
    ms t = get_milli(m);
    return (t/1000)/60;
  }
}

statistics::statistics()
  : data{0}
{
  constexpr int sizeof_bin = NMINUTES*(sizeof(minute) + sizeof(count));
  constexpr int sizeof_milli = 1+NMILLIS*sizeof(milli);
  debug(NDATA);
  debug(sizeof_bin+sizeof_milli);
  assert(NDATA>=(sizeof_bin+sizeof_milli));
  clear();
}

statistics::statistics(uint8_t * src) {
  std::memcpy(data,src,NDATA);
}

void statistics::clear() {
  for(int k=0; k<NDATA; ++k) {
    data[k] = 0;
  }
}

void write_data(uint8_t *c, unsigned char length, int *index, uint8_t *data) {
  int k=*index;
  for(int i=0; i<length; i++)
    data[k++]=c[i];
  *index=k;
}

template<typename T>
void write_data(T input, int *index, uint8_t * data) {
  uint8_t * c=(uint8_t *) &input;
  write_data(c,sizeof(T),index,data);
}

void read_data(uint8_t *c, unsigned char length, int *index, uint8_t *data) {
  assert(length<=8);
  int k=*index;
  for(int i=0; i<length; i++)
    c[i]=data[k++];
  *index=k;
}

template<typename T>
void read_data(T *result, int *index,  uint8_t * data) {
  uint8_t c[sizeof(T)]={0};
  read_data(&c[0],sizeof(T),index,data);
  *result=*((T*)(&c));
}

struct Bin {
  minute m; 
  count c;
};

constexpr int sizeof_bin = sizeof(minute) + sizeof(count);

void write_bin(const Bin &b, const int bin_index, uint8_t *data) {
  int index=sizeof_bin*bin_index;
  debug(bin_index);
  write_data(b.m,&index,data);
  write_data(b.c,&index,data);
}

Bin read_bin(const int bin_index,  uint8_t *data) {
  Bin b={0};
  int index=sizeof_bin*bin_index;
  read_data(&b.m,&index,data);
  read_data(&b.c,&index,data);
  return b;
}

using Indx = uint8_t;
Indx read_milli_index(uint8_t * data) {
  Indx i;
  int index = sizeof_bin*NMINUTES;
  read_data(&i,&index,data);
  return i;
}

void write_milli_indx(Indx indx, uint8_t * data) {
  int index=sizeof_bin*NMINUTES;
  write_data(indx,&index,data);
}

milli read_milli_at_index(Indx indx, uint8_t * data) {
  milli m;
  int index = sizeof_bin*NMINUTES+sizeof(types::milli)*indx+1;
  read_data(&m,&index,data);
  return m;
}

constexpr int sizeof_milli = sizeof(milli);

void write_milli_at_index(milli m, Indx indx, uint8_t * data) {
  int index=sizeof_bin*NMINUTES+sizeof_milli*indx+1;
  write_data(m,&index,data);
}

void statistics::tick() {
  ms t = now();
  minute m = ::get_minute(t);
  debug(m);
  for(int k=0; k<NMINUTES; ++k) {
    Bin b=read_bin(k,data);
    if (b.m == m || b.m == 0) {
      b.m = m;
      b.c++;
      write_bin(b,k,data);
      break;
    }
  }

  milli ml = ::get_milli(t);
  debug(ml);
  Indx indx = read_milli_index(data);
  write_milli_at_index(ml,indx,data);
  indx++;
  if (indx>=NMILLIS)
    indx=0;
  write_milli_indx(indx,data);
}

int statistics::total() {
  int ret=0;
  for(int k=0; k<NMINUTES; ++k) {
    Bin b=read_bin(k,data);
    ret += b.c;
  }
  return ret;
}

int statistics::minute_count()
{
  int ret=0;
  for(int k=0; k<NMINUTES; ++k) {
    Bin b=read_bin(k,data);
    if (b.c!=0)
      ret++;
  }
  return ret;
}

void statistics::get_minute(const int indx, minute *m, count *c)
{
  int k=indx;
  Bin b=read_bin(k,data);
  *m=b.m;
  *c=b.c;
}

types::milli statistics::get_milli(const int k) {
  Indx i0=read_milli_index(data);
  int k2 = i0-k-1;
  if(k2<0)
    k2+=NMILLIS;
  assert(0<=k2&&k2<NMILLIS);
  Indx indx=k2;
  return read_milli_at_index(indx,data);
}

#define MAGIC 78
uint8_t magic = MAGIC;

void statistics::save_eeprom() {
  eeprom e;
  int index=0;
  e.write(index++,magic);
  for(int k=0; k<NDATA; ++k)
    e.write(index++,data[k]);
}

bool statistics::load_eeprom() {
  eeprom e;
  int index=0;
  uint8_t should_be_magic=e.read(index++);
  if (should_be_magic!=magic)
    return false;
  for(int k=0; k<NDATA; ++k)
    data[k]=e.read(index++);
}

bool statistics::operator==(const statistics& other) {
  for(int k=0; k<NDATA; ++k) {
    if (data[k] != other.data[k]) {
      debug(data[k]);
      debug(other.data[k]);
      debug(k);
      return false;
    }
  }
  return true;
}

uint8_t * statistics::getdata(int * Lout) {
  *Lout = sizeof(data)/1; // in bytes
  assert(*Lout == NDATA);
  return (uint8_t*)&data;
}

int statistics::test() {
  artificial = true;
  count c=0; minute m=0;
  
  statistics S;
  {    
    sleep(1000*60*10);
    
    S.tick();
    assert(S.get_milli(0)==1000*60*10);
    assert(S.get_milli(1)==0);
    sleep(20);
    S.tick();
    assert(S.get_milli(0)==1000*60*10+20);
    assert(S.get_milli(1)==1000*60*10);
    sleep(20);
    assert(S.total()==2);
    S.tick();
    assert(S.total()==3);
    assert(S.minute_count()==1);
    
    S.get_minute(0,&m,&c);
    assert(c==3);
    assert(m==10);
    S.get_minute(1,&m,&c);
    assert(c==0);
   
    sleep(1000*65);
    S.tick();
    assert(S.minute_count()==2);
    S.get_minute(0,&m,&c);
    assert(c==3);
    S.get_minute(1,&m,&c);
    assert(c==1);
    assert(m==11);

    sleep(1000*61);
    S.tick();
    assert(S.minute_count()==3);
    S.get_minute(0,&m,&c);
    assert(c==3);
    S.get_minute(2,&m,&c);
    assert(c==1);
    assert(m==12);

    for(int k=0; S.get_milli(k)!=0; ++k)
      debug(S.get_milli(k));

    debug(S.get_milli(1));
    
    assert(S.get_milli(0)!=0);
    assert(S.get_milli(1)!=0);
    assert(S.get_milli(2)!=0);
    assert(S.get_milli(3)!=0);
    assert(S.get_milli(4)!=0);
    assert(S.get_milli(5)==0);

    
    S.save_eeprom();
  }

  int L=0;
  uint8_t * data = S.getdata(&L);
  debug(L);
  debug(NDATA);

  statistics T;
  T.load_eeprom();
  assert(S == T);
  T.get_minute(2,&m,&c);
  assert(c==1);
  assert(m==12);
  
  statistics U(data);
  assert(S == U);
  U.get_minute(2,&m,&c);
  assert(c==1);
  assert(m==12);
  
  debug("statistics::test is good");
  return 0;
}
