#include "message_queue.h"
#include "message.h"
#include "debug.h"

#include <queue>
class message_queue_impl {
  std::queue<received::message> queue;
public:
  message_queue_impl() {}
  virtual ~message_queue_impl() {};
  void push(const received::message &m) {
    queue.push(m);
  }
  received::message wait(bool *ok) const {
    if (queue.empty())
      return received::message();
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
