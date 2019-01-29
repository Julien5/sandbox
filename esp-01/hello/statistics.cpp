#include "statistics.h"
#include "debug.h"
#include "eeprom.h"
#include "freememory.h"
#include <string.h>
#include "clock.h"

#ifndef ARDUINO
#include <vector>
#endif


using namespace types;

statistics::statistics()
  : data{0}
{
  for(int k=0; k<NDATA; ++k)
    assert(data[k]==0);
  constexpr int sizeof_bin = NMINUTES*(sizeof(minute) + sizeof(count));
  constexpr int sizeof_milli = 1+NMILLIS*sizeof(milli);
  assert(NDATA>=(sizeof_bin+sizeof_milli));
  assert(full_total()==0);
  reset();
}

statistics::statistics(uint8_t * src) {
  using namespace std;
  memcpy(data,src,NDATA);
}

#ifndef ARDUINO
statistics statistics::fromHex(const std::string &hex) {
  std::vector<uint8_t> bytes;
  for (unsigned int i = 0; i < hex.length(); i += 2) {
    std::string byteString = hex.substr(i, 2);
    uint8_t byte = (uint8_t) strtol(byteString.c_str(), NULL, 16);
    bytes.push_back(byte);
  }
  bytes.push_back(0);
  return statistics(reinterpret_cast<uint8_t*>(&bytes[0]));
}

std::string statistics::json() const {
    std::string ret;
    ret+="{\n";
    ret+="\"day_total\":"+std::to_string(day_total())+",\n";
    ret+="\"full_total\":"+std::to_string(full_total())+",\n";
    std::string minutes;
    for(int indx=0; indx<minute_count();++indx) {
      types::minute m=0;
      types::count c=0;
      get_minute(indx,&m,&c);
      minutes+="{\"minute\":"+std::to_string(m)+", \"count\":"+std::to_string(c)+"}";
      if (indx<minute_count()-1)
	minutes+=",";
    }
    ret+="\"minutes\":["+minutes+"],\n";
    std::string millis; 
    for(int indx=0; indx<NMILLIS;++indx) {
      types::milli m=get_milli(indx);
      millis+=std::to_string(m);
      if (indx<NMILLIS-1)
	millis+=",";
    }
    ret+="\"millis\":["+millis+"]\n}";
    return ret;
}

std::string statistics::asJson(const std::string &hex) {
  return fromHex(hex).json();
}
#endif

void statistics::reset() {
  // do not reset the big total.
  for(int k=0; k<NDATA-sizeof(types::total); ++k) {
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

void read_data(uint8_t *c, unsigned char length, int *index, const uint8_t *data) {
  assert(length<=8);
  int k=*index;
  for(int i=0; i<length; i++) {
    if (k>=NDATA) {
      DBGTXLN("BAAAAD");
      delay(1000);
      return;
    }
    c[i]=data[k++];
  }
  *index=k;
}

template<typename T>
void read_data(T *result, int *index,  const uint8_t * data) {
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
  write_data(b.m,&index,data);
  write_data(b.c,&index,data);
}

Bin read_bin(const int bin_index,  const uint8_t *data) {
  Bin b={0};
  int index=sizeof_bin*bin_index;
  read_data(&b.m,&index,data);
  read_data(&b.c,&index,data);
  return b;
}

using Indx = uint8_t;
Indx read_milli_index(const uint8_t * data) {
  Indx i;
  int index = sizeof_bin*NMINUTES;
  read_data(&i,&index,data);
  return i;
}

void write_milli_indx(Indx indx, uint8_t * data) {
  int index=sizeof_bin*NMINUTES;
  write_data(indx,&index,data);
}

milli read_milli_at_index(Indx indx, const uint8_t * data) {
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

void write_full_total(total t, uint8_t * data) {
  int index=sizeof_bin*NMINUTES+sizeof_milli*NMILLIS+1;
  write_data(t,&index,data);
}

total read_full_total(const uint8_t * data) {
  int index=sizeof_bin*NMINUTES+sizeof_milli*NMILLIS+1;
  total ret=0;
  read_data(&ret,&index,data);
  return ret;
}

void statistics::tick() {
  minute m = Clock::minutes_since_start();
  for(int k=0; k<NMINUTES; ++k) {
    Bin b=read_bin(k,data);
    if (b.m == m || b.m == 0) {
      b.m = m;
      b.c++;
      write_bin(b,k,data);
      break;
    }
  }

  write_full_total(read_full_total(data)+1,data);

  milli ml = Clock::millis_since_start();
  Indx indx = read_milli_index(data);
  write_milli_at_index(ml,indx,data);
  indx++;
  if (indx>=NMILLIS)
    indx=0;
  write_milli_indx(indx,data);
}

uint16_t statistics::day_total() const {
  int ret=0;
  for(int k=0; k<NMINUTES; ++k) {
    Bin b=read_bin(k,data);
    ret += b.c;
  }
  return ret;
}

types::total statistics::full_total() const {
  return read_full_total(data);
}

int statistics::minute_count() const
{
  int ret=0;
  for(int k=0; k<NMINUTES; ++k) {
    Bin b=read_bin(k,data);
    if (b.c!=0) {
      assert(ret==k);
      ret++;
    }
  }
  return ret;
}

types::minute statistics::last_minute() const {
  minute m;
  count c;
  if (minute_count()==0)
    return 0;
  get_minute(minute_count()-1,&m,&c);
  return m;
}

void statistics::get_minute(const int indx, minute *m, count *c) const
{
  Bin b=read_bin(indx,data);
  *m=b.m;
  *c=b.c;
}

types::milli statistics::get_milli(const int k) const {
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

void statistics::save_eeprom() const {
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
  return true;
}

bool statistics::operator==(const statistics& other) const{
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

uint8_t * statistics::getdata(int * Lout) const {
  *Lout = sizeof(data)/1; // in bytes
  assert(*Lout == NDATA);
  return (uint8_t*)&data;
}

int statistics::test() {
  count c=0; minute m=0;
  
  statistics S;
  {    
    delay(1000L*60*10);
    
    S.tick();
    const milli m0=S.get_milli(0);
    assert(m0!=0);
    assert(m0>1000L*60*10);
    assert(S.get_milli(1)==0);
    delay(20);
    S.tick();
    assert(S.get_milli(0)==m0+20);
    assert(S.get_milli(1)==m0);
    delay(20);
    assert(S.day_total()==2);
    S.tick();
    assert(S.day_total()==3);
    assert(S.minute_count()==1);
    assert(S.last_minute()==10);
    
    S.get_minute(0,&m,&c);
    assert(c==3);
    assert(m==10);
    S.get_minute(1,&m,&c);
    assert(c==0);
   
    delay(1000L*65);
    S.tick();
    assert(S.minute_count()==2);
    S.get_minute(0,&m,&c);
    assert(c==3);
    S.get_minute(1,&m,&c);
    assert(c==1);
    assert(m==11);

    delay(1000L*61);
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
    assert(S.day_total() == 5);
    debug(int(S.full_total()));
    assert(S.full_total() == 5);
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

  
  U.reset();
  assert(U.day_total() == 0);
  assert(U.full_total() > 0);
  
  debug("statistics::test is good");
  return 0;
}
