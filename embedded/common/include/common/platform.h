#pragma once
#include <stdint.h>

/* Note:
 * define abs, max and min macros causes problems with
 * stl headers. 
 */

template <typename T>
T xMin(const T &a, const T &b) {
    if (a < b)
        return a;
    return b;
}

template <typename T>
T xMax(const T &a, const T &b) {
    if (a > b)
        return a;
    return b;
}

#if defined(PC) || defined(ESP8266)
#include <cstddef>
typedef std::size_t size_t;
#endif

#if defined(ARDUINO)
namespace std {
    template <typename T>
    class unique_ptr {
      private:
        const T *_ptr;

      public:
        unique_ptr() {
            _ptr = 0;
        }
        unique_ptr(const T *t) {
            _ptr = t;
        }
        unique_ptr &operator=(unique_ptr &&x) noexcept {
            if (_ptr == x._ptr)
                return *this;
            _ptr = x._ptr;
            x._ptr = nullptr;
            return *this;
        }
        unique_ptr &operator=(const unique_ptr &) = delete;
        explicit operator bool() const {
            return _ptr;
        }
        T *operator->() {
            return const_cast<T *>(_ptr);
        }
        T *operator->() const {
            return get();
        }
        T *get() const {
            return const_cast<T *>(_ptr);
        }
        ~unique_ptr() {
            if (_ptr)
                delete _ptr;
        }
    };
}
#else
#include <memory>
#endif
