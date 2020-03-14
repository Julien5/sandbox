#include "application.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "debug.h"
void serial(void *pvParameters) {
  while (1) {
    application::loop_serial();
  }
}

void wifi(void *pvParameters) {
  while (1) {
    application::loop_wifi();
  }
}

extern "C" {
  void app_main()
  {
    application::setup();
    DBG("free memory:%d\n",debug::freeMemory());
    auto code=xTaskCreate(&serial, "serial_task", 1024*4, nullptr, 2, nullptr);
    DBG("code:%d\n",int(code));
    assert(code>0);
    code=xTaskCreate(&wifi, "wifi_task", 1024*24 + 1000, nullptr, 2, nullptr);
    DBG("code:%d\n",int(code));
    assert(code>0);
  }
}
