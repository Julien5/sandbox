#ifdef ARDUINO
template<typename T>
class Atomic {
  T value;
public:
  Atomic(T v):value(v){};
  T load() const {
    return value;
  }
  T operator=(T val) noexcept {
    value=val;
    return value;
  }
};
#else
#include <atomic>
  template <typename T>
  using Atomic = std::atomic<T>;
#endif
