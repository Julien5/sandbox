#pragma once

#include "message.h"
#include <memory>

class message_queue_impl;

class message_queue {
  std::shared_ptr<message_queue_impl> impl;
public:
  message_queue();
  void push(const received::message &m);
  received::message wait(bool *ok);
};
