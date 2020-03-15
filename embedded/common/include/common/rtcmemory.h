#pragma once

#if defined(ESP8266)
#include <esp_attr.h>
#define TRANSIENT RTC_DATA_ATTR
#else
#define TRANSIENT
#endif
