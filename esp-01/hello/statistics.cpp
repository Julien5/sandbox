#include "statistics.h"
#include "debug.h"
#include "eeprom.h"
#include "freememory.h"

// hi

#ifdef ARDUINO
#include "Arduino.h"
#else
long long millis() {
  return 0;
}
void delay(long long d) {
}
#endif

using namespace types;

namespace {
  bool artificial = false;
  using ms = unsigned long long;
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
 
  
  minute get_minute(const ms &m) {
    ms t = m - t0;
    return (t/1000)/60;
  }

  milli get_milli(const ms &m) {
    ms t = m - t0;
    return t - 60*1000*get_minute(m);
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

void statistics::clear() {
  for(int k=0; k<NDATA; ++k) {
    data[k] = 0;
  }
}

void write_data(char *c, unsigned char length, int *index, char *data) {
  int k=*index;
  for(int i=0; i<length; i++)
    data[k++]=c[i];
  *index=k;
}

template<typename T>
void write_data(T input, int *index, char * data) {
  char * c=(char *) &input;
  write_data(c,sizeof(T),index,data);
}

void read_data(char *c, unsigned char length, int *index, char *data) {
  assert(length<=8);
  int k=*index;
  for(int i=0; i<length; i++)
    c[i]=data[k++];
  *index=k;
}

template<typename T>
void read_data(T *result, int *index, char * data) {
  char c[sizeof(T)]={0};
  read_data(&c[0],sizeof(T),index,data);
  *result=*((T*)(&c));
}

struct Bin {
  minute m; 
  count c;
};

constexpr int sizeof_bin = sizeof(minute) + sizeof(count);

void write_bin(const Bin &b, const int bin_index, char *data) {
  int index=sizeof_bin*bin_index;
  write_data(b.m,&index,data);
  write_data(b.c,&index,data);
}

Bin read_bin(const int bin_index, char *data) {
  Bin b={0};
  int index=sizeof_bin*bin_index;
  read_data(&b.m,&index,data);
  read_data(&b.c,&index,data);
  return b;
}

using Indx = unsigned char;
Indx read_milli_index(char * data) {
  Indx i;
  int index = sizeof_bin*NMINUTES;
  read_data(&i,&index,data);
  return i;
}

void write_milli_indx(Indx indx, char * data) {
  int index=sizeof_bin*NMINUTES;
  write_data(indx,&index,data);
}

/*
milli read_milli_at_index(Indx indx, char * data) {
  milli m;
  int index = 3*NMINUTES+2*indx+1;
  read_data(&m,&index,data);
  return m;
}
*/

constexpr int sizeof_milli = sizeof(milli);

void write_milli_at_index(milli m, Indx indx, char * data) {
  int index=sizeof_bin*NMINUTES+sizeof_milli*indx+1;
  write_data(m,&index,data);
}

void statistics::tick() {
  ms t = now();
  minute m = get_minute(t);
  for(int k=0; k<NMINUTES; ++k) {
    Bin b=read_bin(k,data);
    if (b.m == m || b.m == 0) {
      b.m = m;
      b.c++;
      write_bin(b,k,data);
      break;
    }
  }

  milli ml = get_milli(t);
  Indx indx = read_milli_index(data);
  write_milli_at_index(m,indx,data);
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

int statistics::count_at_minute(const int m)
{
  int k=m;
  Bin b=read_bin(k,data);
  return b.c;
}


#define MAGIC 78
char magic = MAGIC;

void statistics::save() {
  eeprom e;
  int index=0;
  e.write(index++,magic);
  for(int k=0; k<NDATA; ++k)
    e.write(index++,data[k]);
}

bool statistics::load() {
  eeprom e;
  int index=0;
  char should_be_magic=e.read(index++);
  if (should_be_magic!=magic)
    return false;
  for(int k=0; k<NDATA; ++k)
    data[k]=e.read(index++);
}

bool statistics::operator==(const statistics& other) {
  for(int k=0; k<NDATA; ++k) {
    if (data[k] != other.data[k])
      return false;
  }
  return true;
}

char * statistics::getdata(int * Lout) {
  *Lout = sizeof(data)/1; // in bytes
  return (char*)&data;
}

int statistics::test() {
  statistics S;
  {
    artificial = true;
    S.tick();
    sleep(20);
    S.tick();
    sleep(20);
    assert(S.total()==2);
    S.tick();
    assert(S.total()==3);
    assert(S.minute_count()==1);
    assert(S.count_at_minute(0)==3);
    assert(S.count_at_minute(1)==0);
    S.save();
  }

  int L=0;
  char * data = S.getdata(&L);
  debug(L);

  statistics T;
  T.load();
  assert(S == T);
  debug("statistics::test is good");
  return 0;
}
