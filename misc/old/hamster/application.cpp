#include "application.h"
#include "debug.h"
#include "parse.h"
#include "wifi.h"
#include "lcd.h"
#include "tickscounter.h"
#include "freememory.h"
#include "clock.h"
#include <limits.h>
#include "ui.h"
#include "defines.h"
#include "utils.h"
#include "platform.h"
#include "sensor.h"

tickscounter counter;

#ifndef ARDUINO
#define LED_BUILTIN 13
#define HIGH 0x1
#define LOW 0x0
#endif

void reset() {
    counter.save_eeprom_if_necessary();
    display::lcd.print("reset.");
    delay(1000);
#if defined(ARDUINO)
    asm volatile("jmp 0");
#endif
}

void turnLED(u8 pin, u8 mode) {
#ifdef ARDUINO
    pinMode(pin, OUTPUT);
    digitalWrite(pin, mode);
#else
#endif
}

void application::setup() {
#ifdef ARDUINO
    Serial.begin(9600);
    Serial.println("@START");
#endif
    display::lcd.init();
    delay(50);
    display::lcd.print("load eeprom");
    delay(100);
    if (!counter.load_eeprom()) {
        display::lcd.print("eeprom failed");
        delay(500);
    }

    for (int k = 0; k < 5; k++) {
        turnLED(LED_BUILTIN, HIGH);
        delay(50);
        turnLED(LED_BUILTIN, LOW);
        delay(100);
    }
    TRACE();
}

namespace get {
    bool message(wifi::interface &esp, char *buffer) {
        char *internal = 0;
        if (!esp.get("message", &internal)) {
            // retry ?
            return false;
        }
        strncpy(buffer, internal, 16);
        return true;
    }
    bool seconds_until_next_wifi(wifi::interface &esp, u32 *buffer) {
        char *internal = 0;
        if (!esp.get("sunw", &internal)) {
            return false;
        }
        if (sscanf(internal, "%lu", buffer) != 1)
            return false;
        return true;
    }
}

void update_display_wifi(wifi::interface &esp) {
    char line2[17] = {0};
    if (!get::message(esp, line2))
        snprintf(line2, sizeof(line2), "%s", "(no message)");
    display::lcd.print(0, line2);
}

char try_upload_statistics(wifi::interface &esp) {
    u16 length = 0;
    u8 *data = counter.getdata(&length);
    int ret = esp.post("tickscounter", data, length, 0);
    if (ret != 0) {
        char msg[16];
        snprintf(msg, sizeof(msg), "post error: %d", ret);
        display::lcd.print(msg);
        delay(200);
        return 1;
    }
    display::lcd.print("result uploaded");
    counter.reset();
    return 0;
}

bool upload_statistics(wifi::interface &esp) {
    if (counter.empty())
        return true;
    display::lcd.print("uploading...");
    int trials = 3;
    while (trials-- > 0) {
        if (try_upload_statistics(esp) == 0) {
            return true;
        }
        esp.reset();
        esp.join();
    }
    return false;
}

int test_upload(wifi::interface &esp) {
    display::lcd.print(0, "test upload..");
    int d = esp.test_upload();
    if (d != 0) {
        char msg[16];
        snprintf(msg, sizeof(msg), "test error: %d", d);
        display::lcd.print(msg);
        return false;
    } else {
        display::lcd.print(0, "OK");
    }
    return true;
}

bool time_approaches_overflow() {
    return millis() > ULONG_MAX / 2;
}

u32 millis_next_upload = 0;
bool wifi_work() {
    // if upload time has not come, or hamster just did run
    // do not upload (TODO: enable upload and counter at the
    // same time).

    if (millis() <= millis_next_upload) {
        return true;
    }

    if (counter.recently_active()) {
        return true;
    }

    display::lcd.print(0, "wifi...");

    wifi::mock esp;
    //wifi::esp8266 esp;
    turnLED(LED_BUILTIN, LOW);
    if (!esp.enabled()) {
        turnLED(LED_BUILTIN, HIGH);
        display::lcd.print(0, "wifi failed");
        delay(200);
        return false;
    }

    // test upload only at startup.
    if (millis_next_upload == 0) {
        // test
        if (!test_upload(esp))
            return false;
        display::lcd.print(0, "OK1");
        // get T0
        char *internal = 0;
        if (!esp.get("utime", &internal))
            return false;
        display::lcd.print(0, "OK2");
        tickscounter::time_since_epoch T0 = fixed_atoll(internal);
        counter.set_epochtime_at_init(T0);
    }

    if (!upload_statistics(esp))
        return false;

    u32 secs_until_next_wifi = 0;
    if (get::seconds_until_next_wifi(esp, &secs_until_next_wifi))
        millis_next_upload = millis() + 1000L * secs_until_next_wifi;
    else
        millis_next_upload = millis() + 1000L * kDefaultSecondsBetweenWifi;

    update_display_wifi(esp);

    if (time_approaches_overflow())
        reset();

    return true;
}

Clock::ms last_update_display = 0;
int bin_indx = 0;
void update_display_local() {
    Clock::ms t = Clock::millis_since_start();
    if ((t - last_update_display) < 1000)
        return;

    last_update_display = t;

    char line1[17] = {0};
    if (counter.total() > 0) {
        if (bin_indx >= counter.bin_count())
            bin_indx = 0;
        if (counter.recently_active())
            bin_indx = counter.bin_count() - 1;
        bin b = counter.getbin(bin_indx);

        char f1[5] = {0};
        ui::format(counter.total(), f1, sizeof(f1));

        char f2[4] = {0};
        ui::format_seconds(int(b.m_duration / 1000L), f2, sizeof(f2));

        char f3[5] = {0};
        ui::format(int(b.m_count), f3, sizeof(f3));
        DBG(int(b.m_count));
        snprintf(line1, sizeof(line1), "%s.%s.%s.%2d", f1, f2, f3, int(bin_indx + 1));
    } else
        snprintf(line1, sizeof(line1), "no ticks");
    display::lcd.print(line1, 0);

    bin_indx++;
}

void application::loop() {
    static sensor reed_sensor;
    if (counter.save_eeprom_if_necessary()) {
        display::lcd.print("saved to eeprom");
        delay(150);
    }
    update_display_local();
    if (reed_sensor.has_ticked()) {
        counter.tick();
    } else if (!wifi_work()) {
        // Hopefully reset will help.
        // Maybe in case esp8266 got stuck.
        reset();
    }
    delay(1);
}
