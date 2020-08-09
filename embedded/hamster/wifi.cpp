#include "wifi.h"
#include "parse.h"
#include <stdlib.h>
#include "lcd.h"
#include "clock.h"
#include "debug.h"

bool wifi::mock::enabled() const {
    return true;
}

bool wifi::mock::reset() {
    return true;
}

bool wifi::mock::join() {
    return true;
}

bool wifi::mock::get(const char *req, char **response) {
    if (strstr(req, "utime") != NULL) {
        *response = "123456789";
    }
    if (strstr(req, "sunw") != NULL) {
        *response = "50000";
    }
    if (strstr(req, "message") != NULL) {
        *response = "hello";
    }
    return true;
}

int wifi::mock::post(const char *req, const uint8_t *data, const int Ldata, char **response) {
    return 0;
}

int wifi::interface::test_upload() {
    return post("test_upload", 0, 0, 0);
}
