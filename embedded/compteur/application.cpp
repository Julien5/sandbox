#include "application.h"

#include "common/debug.h"

void application::setup() {
  debug::init_serial();
  TRACE();
  DBG("memory:%d\r\n",debug::freeMemory());
}

void application::loop()
{
}
