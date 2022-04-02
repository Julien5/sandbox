#include "httpsender.h"
#include "common/debug.h"
#include "common/time.h"
#include "common/utils.h"
#include "common/serial.h"

#define SKIP

const char espEnablePin = 9;

httpsender::httpsender() {
    // turn on
#ifdef ARDUINO
    LOG("init esp...");
    pinMode(espEnablePin, OUTPUT);
    digitalWrite(espEnablePin, HIGH);
    common::time::delay(common::time::ms(10));
    digitalWrite(espEnablePin, LOW);
    common::time::delay(common::time::ms(10));
    digitalWrite(espEnablePin, HIGH);
    // esp needs 300ms to wake up and about 4s to connect to wifi
    common::time::delay(common::time::ms(500));
    u8 buffer[4] = {0};
    int q = 4;
    while (q--) {
        usize L = common::serial::serial().read(buffer, 4, 1000);
        //utils::dump(buffer, L);
    }
    while (true) {
        usize L = common::serial::serial().read(buffer, 4, 1000);
        if (L == 0)
            break;
        //utils::dump(buffer, L);
    }
    LOG("\r\n");
#endif
}
httpsender::~httpsender() {
    // turn off
#ifdef ARDUINO
    digitalWrite(espEnablePin, LOW);
#endif
}
class data_callback : public wifi::callback {
    int missing_bytes = -1;
    void status(u8 s) {
        DBG("status:%d\r\n", int(s));
        //assert(s == 0);
    }
    void data_length(u16 total_length) {
        DBG("data_length:%d\r\n", int(total_length));
        LOG("memory:%d\r\n", debug::freeMemory());
        missing_bytes = total_length;
    }
    virtual void data(u8 *data, size_t length) {
        //utils::dump(data, length);
        missing_bytes -= length;
        //DBG("missing:%d\r\n", int(missing_bytes));
    }

  public:
    bool done() const {
        return missing_bytes == 0;
    }
};

void wait() {
    common::time::delay(common::time::ms(3500));
}

bool httpsender::post_tickcounter(const u8 *data, const usize &length) {
#if defined(SKIP)
    wait();
    return true;
#endif
    auto t0 = common::time::since_reset();
    int retries = 3;
    while (retries--) {
        data_callback cb;
        m_wifi.post("http://pi:8000/compteur/tickcounter/", data, length, &cb);
        if (cb.done()) {
            DBG("SUCCESS data\r\n");
            DBG("transmit time %d ms\r\n", int(common::time::since_reset().since(t0).value()));
            return true;
        }
        DBG("failed. retry=%d\r\n", retries);
    }
    return false;
}

bool httpsender::post_capacity(const u8 *data, const usize &length) {
#if defined(SKIP)
    wait();
    return true;
#endif
    data_callback cb;
    m_wifi.post("http://pi:8000/compteur/capacity/", data, length, &cb);
    if (cb.done()) {
        return true;
    }
    return false;
}

class epoch_callback : public data_callback {
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
#if defined(SKIP)
    wait();
    return true;
#endif
    int retries = 3;
    while (retries--) {
        epoch_callback cb;
        m_wifi.get("http://pi:8000/epoch", &cb);
#define EPOCH_01_02_2022_00_00 1643670000
        if (cb.epoch < EPOCH_01_02_2022_00_00) {
            DBG("invalid epoch:%ld\r\n", cb.epoch);
        } else {
            *epoch = cb.epoch;
            DBG("SUCCESS epoch:%ld\r\n", cb.epoch);
            return true;
        }
        DBG("failed. retry=%d\r\n", retries);
    }
    return false;
}
