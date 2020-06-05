#include "ticksupdater.h"
#include "adc.h"
#include "common/debug.h"
#include "common/time.h"
#include <math.h>

void print(const histogram::Histogram &H) {
  const auto M=H.maximum();
  const auto m=H.minimum(); 
  const auto TH = H.threshold(5);
  const auto TL = H.threshold(30);
  const auto L = H.argmin(TL,TH+1);
  //  const auto v1 = H.argmax(m,T);
  // const auto v2 = H.argmax(T,M);
  // H.print();
  // DBG("m=%2d TL=%2d min=%2d TH=%2d M=%2d\n",m,TL,L,TH,M);
}

bool TicksReader::calibrated(uint16_t * _TL, uint16_t * _TH) const {
  *_TL=34;
  *_TH=37;
  return true;
  const uint8_t minWidth = 4;
  const auto M=H.maximum();
  const auto m=H.minimum(); 
  if (M-m < minWidth)
    return false;
  const auto L = H.argmin(H.threshold(30),H.threshold(5)+1);
  const auto TH=L+1;
  const auto TL=L;
  if (TH==TL)
    return false;
  /*
  const auto v1 = H.argmax(m,T);
  const auto v2 = H.argmax(T,M);
  // debug(H);
  assert(v1<=v2);
  if (v1==v2)
    return false;
  if (H.count(T)>=H.count(v1))
    return false;
  if (H.count(T)>H.count(v2))
    return false;  
  */
  *_TL=TL;
  *_TH=TH;
  return true;
}

bool TicksReader::take() {
  const auto a = analog::read();
  H.update(a);
  uint16_t TH = 0;
  uint16_t TL = 0;
  assert(TL<=TH);
  // DBG("a(%d)=%d\n",Time::since_reset(),a);
  if (!calibrated(&TL,&TH)) {
    // DBG("not calibrated at time:%d\n",Time::since_reset()/1000);
    return false;
  }
  /* is the value classificable ? */
  if (TL < a && a < TH) {
    // DBG("skip at t=%d with TH=%d a=%d\n",Time::since_reset(),TH,a);
    return false;
  }
  const auto new_value = a>=TH;
  //DBG("new_value:%d last_value:%d\n",new_value,m_last_value);;
  if (new_value==m_last_value)
    return false;
  m_last_value=new_value;
  if (new_value==0) {
    return false;
  }
  print(H);
  //DBG("change at t=%d with T=%d a=%d\n",Time::since_reset(),TH,a);
  return true;
}
