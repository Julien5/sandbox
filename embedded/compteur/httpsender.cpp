#include "httpsender.h"
#include "common/debug.h"
#include "common/time.h"

const char esp_pin = 9;

httpsender::httpsender() {
    // turn on
#ifdef ARDUINO
    pinMode(esp_pin, OUTPUT);
    digitalWrite(esp_pin, HIGH);
#endif
}
httpsender::~httpsender() {
    // turn off
#ifdef ARDUINO
    digitalWrite(esp_pin, LOW);
#endif
}

class callback : public wifi::callback {
    int missing_bytes = -1;
    void status(u8 s) {
        DBG("status:%d\r\n", int(s));
        assert(s == 0);
    }
    void data_length(u16 total_length) {
        DBG("data_length:%d\r\n", int(total_length));
        missing_bytes = total_length;
    }
    virtual void data(u8 *data, size_t length) {
        missing_bytes -= length;
    }

  public:
    bool done() const {
        return missing_bytes == 0;
    }
};

bool httpsender::post_tickcounter(const u8 *data, const usize &length) {
    TRACE();
    auto t0 = common::time::since_reset();
    callback cb;
    m_wifi.post("http://pi:8000/compteur/tickcounter/data/", data, length, &cb);
    DBG("transmit time %d ms\r\n", int(common::time::since_reset().since(t0).value()));
    return cb.done();
}

class epoch_callback : public callback {
  public:
    u64 epoch = 0;
    void data(u8 *data, size_t length) {
        for (usize k = 0; k < length; ++k) {
            char n = data[k] - '0';
            auto ischar = 0 <= n && n <= 9;
            if (!ischar)
                continue;
            epoch = 10 * epoch + n;
        }
    }
};

bool httpsender::get_epoch(u64 *epoch) {
    TRACE();
    epoch_callback cb;
    m_wifi.get("http://pi:8000/epoch", &cb);
#define EPOCH_01_02_2022_00_00 1643670000
    if (cb.epoch < EPOCH_01_02_2022_00_00) {
        DBG("invalid epoch:%ld\r\n", cb.epoch);
        return false;
    }
    *epoch = cb.epoch;
    return true;
}
