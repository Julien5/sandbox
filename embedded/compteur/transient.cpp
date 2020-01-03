#include "transient.h"
#include "rtcmemory.h"
#include "debug.h"

bool transient::load(transient *t) {
  rtcmemory RTC;
  bool ok=RTC.read(t,sizeof(*t));
  // rtcmemory::reset();
  if (!ok || t->magic!=transient::MAGIC) {
    DBG("error: could not read transient\n");
    DBG("\t MAGIC: %d\n",transient::MAGIC);
    DBG("\t magic: %d\n",t->magic);
    *t=transient();
    return false;
  }
  return true;
}

bool transient::save(transient *t) {
    rtcmemory RTC;
    return RTC.write(t,sizeof(*t));
}
