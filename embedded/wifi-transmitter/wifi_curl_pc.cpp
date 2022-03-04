#include "wifi_curl.h"
#include "common/debug.h"
#include "common/utils.h"
#include "common/stringawaiter.h"
#include <cstdlib>
#include <string>
#include <fstream>
#include <vector>
#include <iterator>
#include <strings.h>
#include <string.h>
#include "common/platform.h"
#include "common/time.h"

#include <fstream>

wifi::wifi_curl::wifi_curl(){};
wifi::wifi_curl::~wifi_curl(){};

const std::string kDataFile = "/tmp/wifi.curloutput.internal";

size_t remain(size_t buffer_size, size_t pos) {
    if (pos >= buffer_size)
        return 0;
    return buffer_size - pos;
}

bool file_exists(const std::string &filename) {
    std::ifstream infile(filename.c_str());
    return infile.good();
}

std::string command(const std::string &method, const char *req, const u8 *data = nullptr, const int Ldata = 0) {
    std::string curl = "curl --http0.9 -i --raw -s -X ";
    std::string cmd = curl + method + " ";
    if (std::getenv("TEST_CURL_SH")) {
        const std::string test_curl = std::string(std::getenv("TEST_CURL_SH"));
        cmd = test_curl + " time:" + std::to_string(common::time::since_reset().value()) + " " + method + " ";
        if (!file_exists(test_curl)) {
            DBG("failed: could not find: %s req:%s cmd:%s\r\n", test_curl.c_str(), req, cmd.c_str());
            return std::string();
        }
    }

    if (data && Ldata) {
        cmd += "--data-binary \"@data.bin\"";
        cmd += " ";
    }
    cmd += std::string(req);
    cmd += " --output " + kDataFile;
    return cmd;
}

int exe(const std::string &method, const char *req, wifi::callback *cb, const u8 *data = nullptr, const int Ldata = 0) {
    // remove output file (cleanup)
    if (file_exists(kDataFile))
        std::remove(kDataFile.c_str());
    std::string cmd = command(method, req, data, Ldata);
    if (cmd.empty()) {
        return 4;
    }
    if (data && Ldata) {
        std::ofstream f;
        f.open("data.bin", std::ios::out | std::ios::binary | std::ios::trunc);
        f.write((char *)data, Ldata);
        f.close();
    }
    DBG("exe: %s\n", cmd.c_str());
    std::system(cmd.c_str());
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1000ms);
    errno = 0;
    auto f = fopen(kDataFile.c_str(), "rb");
    auto code = errno;
    cb->status(code);
    if (!f) {
        DBG("failed to find: %s code:%d\n", kDataFile.c_str(), int(code));
        assert(code != 0);
        return code;
    }

    u8 buffer[16 * 1024];
    memset(buffer, 0, sizeof(buffer));
    size_t buffer_size = 0;
    while (true) {
        u8 recv_buf[16];
        memset(recv_buf, 0, sizeof(recv_buf));
        int r = fread(recv_buf, 1, sizeof(recv_buf), f);
        if (r > 0) {
            if (buffer_size + r >= sizeof(buffer))
                break; // max size exceeded
            memcpy(buffer + buffer_size, recv_buf, r);
            buffer_size += r;
        } else {
            // DBG("done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
            break;
        }
    }
    fclose(f);

    cb->data_length(buffer_size);

    // send chunk-wise so that arduino read buffer does not overflow.
    if (cb && buffer_size > 0) {
        u8 buf[32];
        size_t pos = 0;
        while (remain(buffer_size, pos) > 0) {
            size_t size_copy = xMin(sizeof(buf), remain(buffer_size, pos));
            memcpy(buf, buffer + pos, size_copy);
            cb->data((u8 *)buf, size_copy);
            pos += size_copy;
        }
    }

    //std::remove(kDataFile);
    if (data && Ldata)
        std::remove("data.bin");
    TRACE();
    return 0;
}

int wifi::wifi_curl::get(const char *req, callback *r) {
    return exe("GET", req, r);
}

int wifi::wifi_curl::post(const char *req, const u8 *data, const u16 Ldata, callback *r) {
    return exe("POST", req, r, data, Ldata);
}
