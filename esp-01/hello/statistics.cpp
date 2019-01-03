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
  : bins{{0,0}},
    millis{}
{
}

void statistics::clear() {
  for(int k=0; k<NMINUTES; ++k) {
    bins[k] = {0,0};
  }
  for(int k=0; k<NMILLIS; ++k) {
    millis[k] = {0};
  }
}

void statistics::tick() {
  ms t = now();
  minute m = get_minute(t);
  for(int k=0; k<NMINUTES; ++k) {
    if (bins[k].m == m || bins[k].m == 0) {
      bins[k].m = m;
      bins[k].c++;
      break;
    }
  }

  const int index = 0;
  milli ml = get_milli(t);
  millis[index].m = ml;
}

#define MAGIC 78

void write_to_eeprom(char *c, unsigned char length, int *index) {
  int k=*index;
  for(int i=0; i<length; i++)
    eeprom().write(k++,c[i]);
  *index=k;
}

// TODO: template
void write_int_to_eeprom(int input, int *index) {
  char * c=(char *) &input;
  write_to_eeprom(c,sizeof(int),index);
}

/*
void write_time_to_eeprom(statistics::time input, int *index) {
  char * c=(char *) &input;
  write_to_eeprom(c,sizeof(statistics::time),index);
}
*/

void read_from_eeprom(char *c, unsigned char length, int *index) {
  assert(length<=8);
  int k=*index;
  for(int i=0; i<length; i++)
    c[i]=eeprom().read(k++);
  *index=k;
}

#ifdef ARDUINO
#define CHAR_LENGTH 1
#define INT_LENGTH 2
#define TIME_LENGTH 4
#else
#define CHAR_LENGTH 1
#define INT_LENGTH 4
#define TIME_LENGTH 4
#endif


void read_int_from_eeprom(int *result, int *index) {
  assert(sizeof(int)==INT_LENGTH);
  char c[INT_LENGTH]={0};
  read_from_eeprom(&c[0],INT_LENGTH,index);
  *result=*((int*)(&c));
}

/*
void read_time_from_eeprom(statistics::time *result, int *index) {
  assert(sizeof(statistics::time)==TIME_LENGTH);
  char c[TIME_LENGTH]={0};
  read_from_eeprom(&c[0],TIME_LENGTH,index);
  *result=*((statistics::time*)(&c));
}
*/

/*
bool statistics::load() {
  int k=0;
  char magic_number=eeprom().read(k++);
  if (magic_number != MAGIC) {
    return false;
  }
  read_int_from_eeprom(&index,&k);
  read_int_from_eeprom(&total_count,&k);
  for(int i=0; i<NTICKS; i++) {
    read_int_from_eeprom(&(ticks.delta[i]),&k);
  }
  read_time_from_eeprom(&(ticks.t0),&k);
  return true;
}
*/


char * statistics::getdata(int * Lout) {
  return 0;
  /*
  if (get_count()==0) {
    return 0;
  }
  const int t0=ticks.t0;
  assert(t0>0);
  assert(ticks.t0==t0);
  *Lout = sizeof(ticks)/1; // in bytes
  return (char*)&ticks;
  */
}

int statistics::test() {
  debug("good");
  return 0;
}
