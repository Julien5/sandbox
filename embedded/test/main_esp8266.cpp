#include "application.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void maintask_esp8266(void *pvParameters) {
    while (1) {
        application::loop();
    }
}

extern "C" {
void app_main() {
    application::setup();
    xTaskCreate(&maintask_esp8266, "maintask_esp8266", 1024 * 16, nullptr, 2, nullptr);
}
}
