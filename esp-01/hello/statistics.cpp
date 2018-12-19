#include "statistics.h"
#include "debug.h"
#include "eeprom.h"

statistics::statistics()
  : index(0)
  , total_count(0)
{
  if (!load()) {
    debug("failed to load");
    clear();
  }
}

void statistics::clear() {
  for(int k=0; k<NTICKS; ++k) {
    ticks.delta[k]=0;
  }
  ticks.t0=0;
  index=0;
  save();
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

void write_time_to_eeprom(statistics::time input, int *index) {
  char * c=(char *) &input;
  write_to_eeprom(c,sizeof(statistics::time),index);
}

void statistics::save() {
  int k=0;
  eeprom().write(k++,MAGIC);  
  write_int_to_eeprom(index,&k);
  write_int_to_eeprom(total_count,&k);
  for(int i=0; i<NTICKS; i++)
    write_int_to_eeprom(ticks.delta[i],&k);
  write_time_to_eeprom(ticks.t0,&k);
}

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
#define TIME_LENGTH 8
#endif

void read_int_from_eeprom(int *result, int *index) {
  assert(sizeof(int)==INT_LENGTH);
  char c[INT_LENGTH]={0};
  read_from_eeprom(&c[0],INT_LENGTH,index);
  *result=*((int*)(&c));
}
void read_time_from_eeprom(statistics::time *result, int *index) {
  assert(sizeof(statistics::time)==TIME_LENGTH);
  char c[TIME_LENGTH]={0};
  read_from_eeprom(&c[0],TIME_LENGTH,index);
  *result=*((statistics::time*)(&c));
}

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

int statistics::elapsed(const time m) const {
  return int(m - ticks.t0);
}

void statistics::increment_count(const time m, int incr) {
  if (ticks.t0==0) {
    ticks.t0=m;
  }
  assert(ticks.t0!=0);
  const time delta = m - ticks.t0;
  assert(index<sizeof(ticks.delta)/sizeof(ticks.delta[0]));
  ticks.delta[index++]=delta;
  total_count = total_count + incr;
}
void statistics::increment_count(const time m) {
  increment_count(m,1);
}

int statistics::get_count() const {
  int ret=0;
  for(int t=0; t<NTICKS; ++t) {
    if (ticks.delta[t]!=0)
      ret++;
  }
  return ret;
}

int statistics::get_total_count() const {
  return total_count;
}
#include "HardwareSerial.h"
char * statistics::getdata(time m, int * Lout) {
  if (get_count()==0) {
    Serial.print("no data");
    return 0;
  }
  Serial.print(get_count()); Serial.println(" data");
  const int t0=ticks.t0;
  assert(t0>0);
  assert(ticks.t0==t0);
  *Lout = sizeof(ticks)/1; // in bytes
  return (char*)&ticks;
}

int statistics::test() {
  const time sec=1000;
  const time min=60*sec;
  time t0=2*min;
  {
    statistics s;
    s.clear();
    for(int k=0; k<NTICKS; ++k)
      s.increment_count(t0+k);
    for(int k=0; k<NTICKS; ++k) {
      assert(s.ticks.delta[k]==k);
    }
    assert(s.ticks.t0==t0);
    s.save();
  }
  debug("ok");
  {
    statistics s;
    for(int k=0; k<NTICKS; ++k){
      assert(s.ticks.delta[k]==k);
    }
    assert(s.ticks.t0==t0);
    int L=0;
    char * d = s.getdata(t0+60,&L);
    assert(s.ticks.t0==t0);
  }
  debug("good");
  return 0;
}
