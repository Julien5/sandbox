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
  
  statistics::minute get_minute(const ms &m) {
    ms t = m - t0;
    return (t/1000)/60;
  }

  statistics::milli get_milli(const ms &m) {
    ms t = m - t0;
    return t - 60*1000*get_minute(m);
  }
}

statistics::statistics()
  : data{{0,0},{{0},0}}
{
  clear();
}

void statistics::clear() {
  for(int k=0; k<NMINUTES; ++k) {
    data.bins[k] = {0,0};
  }
  for(int k=0; k<NMILLIS; ++k) {
    data.millis.m[k] = 0;
  }
  data.millis.index = 0;
}

void statistics::tick() {
  ms t = now();
  minute m = get_minute(t);
  for(int k=0; k<NMINUTES; ++k) {
    if (data.bins[k].m == m || data.bins[k].m == 0) {
      data.bins[k].m = m;
      data.bins[k].c++;
      break;
    }
  }

  milli ml = get_milli(t);
  data.millis.m[data.millis.index++] = ml;
  if (data.millis.index>=NMILLIS)
    data.millis.index=0;
}

void write_to_eeprom(char *c, unsigned char length, int *index) {
  int k=*index;
  for(int i=0; i<length; i++)
    eeprom().write(k++,c[i]);
  *index=k;
}

template<typename T>
void write_to_eeprom(T input, int *index) {
  char * c=(char *) &input;
  write_to_eeprom(c,sizeof(T),index);
}

void read_from_eeprom(char *c, unsigned char length, int *index) {
  assert(length<=8);
  int k=*index;
  for(int i=0; i<length; i++)
    c[i]=eeprom().read(k++);
  *index=k;
}

template<typename T>
void read_from_eeprom(T *result, int *index) {
  char c[sizeof(T)]={0};
  read_from_eeprom(&c[0],sizeof(T),index);
  *result=*((T*)(&c));
}

#define MAGIC 78
char magic = MAGIC;

void statistics::save() {
  int index=0;
  write_to_eeprom(magic,&index);
  for(int k=0; k<NMINUTES; ++k) {
    write_to_eeprom(data.bins[k].m,&index);
    write_to_eeprom(data.bins[k].c,&index);
  }
  write_to_eeprom(data.millis.index,&index);
  for(int k=0; k<NMILLIS; ++k) {
    write_to_eeprom(data.millis.m[k],&index);
  }
}

bool statistics::load() {
  int index=0;
  char should_be_magic;
  read_from_eeprom(&should_be_magic,&index);
  if (should_be_magic!=magic)
    return false;
  for(int k=0; k<NMINUTES; ++k) {
    read_from_eeprom(&data.bins[k].m,&index);
    read_from_eeprom(&data.bins[k].c,&index);
  }
  read_from_eeprom(&data.millis.index,&index);
  for(int k=0; k<NMILLIS; ++k) {
    read_from_eeprom(&data.millis.m[k],&index);
  }
}

bool statistics::operator==(const statistics& other) {
  for(int k=0; k<NMINUTES; ++k) {
    if (data.bins[k].m != other.data.bins[k].m)
      return false;
    if (data.bins[k].c != other.data.bins[k].c)
      return false;
  }
  if (data.millis.index != other.data.millis.index)
    return false;
  for(int k=0; k<NMILLIS; ++k) {
    if (data.millis.m[k] != other.data.millis.m[k])
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
  for(int k=0; k<L; ++k) {
    debug(k);
    debug(char(data[k]));
  }

  statistics T;
  T.load();
  assert(S == T);
  debug("statistics::test is good");
  return 0;
}
