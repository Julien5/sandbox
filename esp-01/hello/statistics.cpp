#include "statistics.h"
#include "debug.h"
#include "eeprom.h"
#include "freememory.h"

#ifdef ARDUINO
#include "Arduino.h"
#else
long long millis() {
  return 0;
}
void delay(long long d) {
}
#endif

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
 
  using minute = unsigned int;
  using count = unsigned char;
  using milli = unsigned int;
  
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

void write_bin(const Bin &b, const int bin_index, char *data) {
  int index=3*bin_index;
  write_data(b.m,&index,data);
  write_data(b.c,&index,data);
}

Bin read_bin(const int bin_index, char *data) {
  Bin b={0};
  int index=3*bin_index;
  read_data(&b.m,&index,data);
  write_data(&b.c,&index,data);
  return b;
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
