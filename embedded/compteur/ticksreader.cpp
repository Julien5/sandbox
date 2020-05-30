#include "ticksupdater.h"
#include "adc.h"
#include "common/debug.h"
#include <math.h>

bool TicksReader::calibrated(uint16_t * _TL, uint16_t * _TH) const {
  const uint8_t minWidth = 4;
  const auto M=H.maximum();
  const auto m=H.minimum(); 
  if (M-m < minWidth)
    return false;
  if (m==H.least())
    return false;
  if (M==H.least())
    return false;
  const auto T = H.least();
  const auto TL = T-1;
  const auto TH = T+1;
  // DBG("m=%2d M=%2d TL=%2d TH=%2d least=%2d\n",m,M,TL,TH,H.least());
  assert(TH>=TL);
  assert(TL>=m);
  assert(TH<=M);
  if (TH<=m)
    return false;
  if (TL>=M)
    return false;
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
  if (!calibrated(&TL,&TH))
    return false;
   /* is the value classificable ? */
  if (TL <= a && a <= TH) {
    return false;
  }
  const auto new_value = a>TH;
  if (new_value==m_last_value)
    return false;
  m_last_value=new_value;
  if (new_value==1) {
    return true;
  }
  return false;
}
