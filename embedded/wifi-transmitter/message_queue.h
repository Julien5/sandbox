#pragma once

#include "message.h"
#include <memory>

class message_queue_impl;

class message_queue {
  std::shared_ptr<message_queue_impl> impl;
public:
  message_queue();
  void push(const message &m);
  message wait(bool *ok);
};
