#include "application.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "common/debug.h"

void serial(void *pvParameters) {
  while (1) {
    transmitter::loop_serial();
  }
}

extern "C" {
  void app_main()
  {
    transmitter::setup();
    DBG("free memory:%d\n",debug::freeMemory());
    auto code=xTaskCreate(&serial, "serial_task", 1024*4, nullptr, 2, nullptr);
    DBG("code:%d\n",int(code));
    assert(code>0);
  }
}
