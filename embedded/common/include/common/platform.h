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

template<typename T>
T xMax(const T &a, const T &b) {
  if (a>b)
    return a;
  return b;
}

#if defined(DEVHOST) || defined(ESP8266)
#include <cstddef>
typedef std::size_t size_t;
#endif

#if defined(ARDUINO)
namespace std {
  template<typename T>
  class unique_ptr {
  private:
    T* _ptr;
  public:
    unique_ptr() {
      _ptr = 0;
    }
    unique_ptr(T * t) {
      _ptr = t;
    }
    explicit operator bool() const {
      return _ptr;
    }
    T* operator->() const {
      return _ptr;
    }
    ~unique_ptr() {
      delete _ptr;
    }
  };
}
#else
#include <memory>
#endif
