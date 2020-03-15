#include "message_queue.h"
#include "message.h"
#include "common/debug.h"
#include <mutex>
#include <queue>

class message_queue_impl {
  std::queue<received::message> queue;
  mutable std::mutex mtx;
public:
  message_queue_impl() {}
  virtual ~message_queue_impl() {};
  void push(const received::message &m) {
    const std::lock_guard<std::mutex> lock(mtx);
    queue.push(m);
  }
  received::message wait(bool *ok) const {
    const std::lock_guard<std::mutex> lock(mtx);
    if (queue.empty())
      return received::message();
    *ok=true;
    auto ret=queue.front();
    queue.pop();
    return ret;    
  }
};

message_queue::message_queue():impl(new message_queue_impl) {
}

void message_queue::push(const received::message &m) {
  return impl->push(m);
}

received::message message_queue::wait(bool *ok) {
  return impl->wait(ok);
}
