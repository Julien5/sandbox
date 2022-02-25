#include "common/serial.h"
#include "common/debug.h"
#include "common/utils.h"
#include <vector>
#include <cstring>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include "crc.h"

struct serial_buffer {
    std::vector<u8> data;
    std::mutex mtx;
    std::condition_variable cond_var;

  public:
    usize read(u8 *buffer, size_t buffer_size, u16 timeout) {
        std::unique_lock<std::mutex> lock(mtx);
        // DBG("reading from buffer %p, data %p.\r\n", this, &data);
        if (data.empty()) {
            cond_var.wait_for(lock, std::chrono::milliseconds(timeout));
        }
        if (data.empty()) {
            DBG("timeout\n");
            return 0;
        }
        const size_t Lread = xMin(buffer_size, data.size());
        memcpy(buffer, data.data(), Lread);
        data.erase(data.begin(), data.begin() + Lread);
        //DBG("buffer size: %d\r\n", int(data.size()));
        return Lread;
    }
    usize write(u8 *buffer, size_t buffer_size) {
        const std::unique_lock<std::mutex> lock(mtx);
        for (size_t i = 0; i < buffer_size; ++i)
            data.push_back(buffer[i]);
        //DBG("buffer size: %d in %p\r\n", int(data.size()), &data);
        cond_var.notify_one();
        return buffer_size;
    }
};

namespace {
    serial_buffer s_rxbuffer;
    serial_buffer s_txbuffer;
}

#include <map>

struct G {
    std::mutex mtx;
    std::map<common::serial *, int> m;
    static int counter;
    int get(common::serial *s) {
        std::unique_lock<std::mutex> lock(mtx);
        auto ret = m[s];
        return ret;
    }
    void record(common::serial *s) {
        std::unique_lock<std::mutex> lock(mtx);
        m[s] = counter++;
    }
};

namespace {
    G s_map;
}

int G::counter = 0;

common::serial::serial() {
    s_map.record(this);
}

usize common::serial::read(u8 *buffer, usize buffer_size, u16 timeout) {
    auto &buf = s_map.get(this) == 0 ? s_rxbuffer : s_txbuffer;
    const auto Lread = buf.read(buffer, buffer_size, timeout);
    if (Lread == 0) {
        return Lread;
    }
    DBG("Lread:%d\r\n", int(Lread));
    crc::CRC8(&rx_crc8, buffer, Lread);
    return Lread;
}

void write_file(u8 *buffer, size_t buffer_size) {
    std::fstream f("/tmp/serial.write", std::ios::out | std::ios::binary | std::ios::app);
    f.write(reinterpret_cast<char *>(buffer), buffer_size);
}

size_t common::serial::write(u8 *buffer, size_t buffer_size) {
    write_file(buffer, buffer_size);
    auto &buf = s_map.get(this) == 0 ? s_txbuffer : s_rxbuffer;
    buf.write(buffer, buffer_size);
    crc::CRC8(&tx_crc8, buffer, buffer_size);
    return buffer_size;
}
