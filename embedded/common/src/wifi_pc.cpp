#include "common/time.h"
#include "common/wifi.h"
#include "common/serial.h"
#include "common/debug.h"
#include "common/platform.h"

#include <string.h>
#define BLOCK_LENGTH 32

namespace wifi {
    std::unique_ptr<common::serial> S;
    wifi::wifi() {
        assert(!S);
        S = std::unique_ptr<common::serial>(new common::serial);
        debug::address_range("wifi:", this, sizeof(*this));
    }

    wifi::~wifi() {
    }

    int read_wifi_response(common::serial *S, callback *r) {
        bool ok = false;
        TRACE();
        // at this point, the request has been read, which is fast
        if (!S->wait_for_begin(100))
            return 1;

        // at this point, 3 things happen on the peer:
        // 1. the connection to the URL is being established, request
        // 2. send request
        // 3. receive response
        // Network requests are slow.
        // If all goes well, status=0 is sent.
        u8 status = 0;
        ok = S->read_until(&status, sizeof(status), 5000);
        if (!ok) {
            return 2;
        }
        r->status(status);
        if (status != 0) {
            return 3;
        }

        // now we prepare receiving the data: first the size, then the data.
        // the timeout is short since the data are complete on the peer.
        u16 size = 0;
        ok = S->read_until(reinterpret_cast<u8 *>(&size), sizeof(size), 10);
        if (!ok) {
            return 4;
        }
        r->data_length(size);

        size_t nread = 0;
        while (nread < size) {
            u8 buffer[BLOCK_LENGTH];
            const size_t L = xMin(sizeof(buffer), size - nread);
            auto ok = S->read_until(buffer, L, 10);
#if !defined(NDEBUG)
            DBG("memory:%d\r\n", debug::freeMemory());
#endif
            if (!ok) {
                DBG("nread:%d\r\n", nread);
                return 5;
            }
            r->data(buffer, L);
            nread += L;
        }
        ok = S->check_end();
        return ok ? 0 : 6;
    }

    int wifi::get(const char *url, callback *r) {
        const char command = 'G';
        const char *data = nullptr;
        const u16 Ldata = 0;
        //                G   http......    0 + Ldata         + data...
        u16 Ltotal = 1 + strlen(url) + 1 + sizeof(Ldata) + Ldata;

        S->begin();
        S->write((u8 *)&Ltotal, sizeof(Ltotal));
        S->write((u8 *)&command, sizeof(command));
        S->write((u8 *)url, strlen(url) + 1);
        S->write((u8 *)&Ldata, sizeof(Ldata));
        assert(!data);
        S->end();
        return read_wifi_response(S.get(), r);
    }

    int wifi::post(const char *url, const u8 *data, const u16 Ldata, callback *r) {
        const char command = 'P';
        //                G   http......    0 + Ldata         + data...
        const u16 Ltotal = 1 + strlen(url) + 1 + sizeof(Ldata) + Ldata;
        TRACE();
        S->begin();
        S->write((u8 *)&Ltotal, sizeof(Ltotal));
        S->write((u8 *)&command, sizeof(command));
        S->write((u8 *)url, strlen(url) + 1);
        S->write((u8 *)&Ldata, sizeof(Ldata));
        S->write((u8 *)data, Ldata);
        S->end();
        TRACE();
        int ret = read_wifi_response(S.get(), r);
        return ret;
    }
}
