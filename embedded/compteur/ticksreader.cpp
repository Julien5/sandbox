#include "compteur.h"
#include "adc.h"
#include "common/debug.h"
#include "common/time.h"
#include <math.h>

bool
TicksReader::calibrated(uint16_t* _TL, uint16_t* _TH) const
{
  const uint8_t minWidth = 4;
  const auto M = H.maximum();
  const auto m = H.minimum();
  if (M - m < minWidth)
    return false;
  const auto T0 = H.threshold(5);
  if (T0 == m || T0 == M)
    return false;
  const auto v1 = H.argmax(m, T0 - 1);
  const auto v2 = H.argmax(T0, M);
  if (v1 == v2)
    return false;
  const auto v = H.argmin(v1, v2);
  if (v == v1 || v == v2)
    return false;
  const auto TH = v + 1;
  const auto TL = v - 1;
  *_TL = TL;
  *_TH = TH;
  return true;
}

bool
TicksReader::take()
{
  const auto a = analog::read();
  DBG("time:%4d s analog:%d\r\n", int(Time::since_reset() / 1000), int(a));
  H.update(a);
  H.print();
  uint16_t TH = 0;
  uint16_t TL = 0;
  assert(TL <= TH);
  if (!calibrated(&TL, &TH)) {
    return false;
  }
  /* is the value classificable ? */
  if (TL < a && a < TH) {
    return false;
  }
  const auto new_value = a >= TH;
  if (new_value == m_last_value)
    return false;
  m_last_value = new_value;
  if (new_value == 0) {
    return false;
  }
  return true;
}
