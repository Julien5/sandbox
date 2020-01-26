#pragma once
#include <stdint.h>

/* Note:
 * define abs, max and min macros causes problems with
 * stl headers. 
 */

template<typename T>
T xMin(const T &a, const T &b) {
  if (a<b)
    return a;
  return b;
}

#ifdef DEVHOST
#include <cstddef>
typedef std::size_t size_t;
#endif
