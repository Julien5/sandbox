#include "common/wifi.h"
#include "common/serial.h"
#include "common/debug.h"
#include "common/platform.h"

#include <string.h>
#define BLOCK_LENGTH 32

namespace wifi {
    std::unique_ptr<serial> S;
    wifi::wifi() {
        assert(!S);
        S = std::unique_ptr<serial>(new serial);
        debug::address_range("wifi:", this, sizeof(*this));
    }

    wifi::~wifi() {
    }

    int read_wifi_response(serial *S, callback *r) {
        bool ok = false;
        while (true) { // FIXME: timeout.
            ok = S->wait_for_begin();
            if (ok)
                break;
        }
        uint8_t status = 0;
        // at this point, the request has been read and is being executed.
        // Network request are slow.
        // Timeout is set to 100ms.
        // lets say 20 secs => 200 trials
        uint8_t trials = 200;
        ok = false;
        while (!ok && trials--)
            ok = S->read_until(&status, sizeof(status));
        if (!ok) {
            assert(0);
            r->crc(false);
            return 1;
        }
        r->status(status);

        uint16_t size = 0; // FIXME: ntoh
        ok = S->read_until(reinterpret_cast<uint8_t *>(&size), sizeof(size));

        if (!ok) {
            assert(0);
            r->crc(false);
            return 2;
        }
        r->data_length(size);

        size_t nread = 0;
        while (nread < size) {
            uint8_t buffer[BLOCK_LENGTH];
            const size_t L = xMin(sizeof(buffer), size - nread);
            ok = S->read_until(buffer, L);
#if !defined(NDEBUG)
            auto free = debug::freeMemory();
            if (free < 500)
                DBG("memory:%d\r\n", debug::freeMemory());
#endif
            if (!ok) {
                DBG("nread:%d\r\n", nread);
                assert(0);
                r->crc(false);
                return 3;
            }
            r->data(buffer, L);
            nread += L;
        }
        ok = S->check_end();
        assert(ok);
        r->crc(ok);
        return 0;
    }

    int wifi::get(const char *url, callback *r) {
        const char command = 'G';
        const char *data = nullptr;
        const uint16_t Ldata = 0;
        //                G   http......    0 + Ldata         + data...
        uint16_t Ltotal = 1 + strlen(url) + 1 + sizeof(Ldata) + Ldata;

        S->begin();
        S->write((uint8_t *)&Ltotal, sizeof(Ltotal));
        S->write((uint8_t *)&command, sizeof(command));
        S->write((uint8_t *)url, strlen(url) + 1);
        S->write((uint8_t *)&Ldata, sizeof(Ldata));
        assert(!data);
        S->end();
        return read_wifi_response(S.get(), r);
    }

    int wifi::post(const char *url, const uint8_t *data, const uint16_t Ldata, callback *r) {
        const char command = 'P';
        //                G   http......    0 + Ldata         + data...
        const uint16_t Ltotal = 1 + strlen(url) + 1 + sizeof(Ldata) + Ldata;
        S->begin();
        S->write((uint8_t *)&Ltotal, sizeof(Ltotal));
        S->write((uint8_t *)&command, sizeof(command));
        S->write((uint8_t *)url, strlen(url) + 1);
        S->write((uint8_t *)&Ldata, sizeof(Ldata));
        S->write((uint8_t *)data, Ldata);
        S->end();
        return read_wifi_response(S.get(), r);
    }
}
