#include "message.h"

#include <string.h>

message::message() {
  memset(data,0,sizeof(data)/sizeof(data[0]));
}
