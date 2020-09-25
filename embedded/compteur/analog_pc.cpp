#include "analog.h"
#include <cassert>
#include "common/debug.h"

#define RECORD

#ifdef RECORD

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

namespace record {
    std::string read_file(const std::string &fileName) {
        std::ifstream ifs(fileName.c_str(), std::ios::in | std::ios::ate);
        std::ifstream::pos_type s = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::vector<char> bytes(s);
        ifs.read(bytes.data(), s);
        return std::string(bytes.data(), s);
    }

    std::vector<std::string> split(const std::string &s, const std::string &sep) {
        size_t beg = 0;
        size_t end = 0;
        std::vector<std::string> ret;
        while (true) {
            end = s.find(sep, beg);
            auto part = s.substr(beg, end == std::string::npos ? end : end - beg);
            ret.push_back(part);
            if (end == std::string::npos)
                break;
            beg = end + 1;
            end = 0;
        }
        return ret;
    }

    std::vector<std::string> lines(const std::string &s) {
        auto ret = split(s, "\n");
        return ret;
    }

    std::vector<u16> numbers(const std::vector<std::string> &lines) {
        std::vector<u16> ret;
        for (auto &line : lines) {
            if (!line.empty()) {
                const auto P = split(line, " ");
                ret.push_back(std::stoi(P[1]));
            }
        }
        return ret;
    }

    std::vector<u16> s_numbers;
    size_t counter = 0;
    void get_data() {
        std::string s;
        auto n = numbers(lines(read_file("/tmp/data.csv")));
        s_numbers = n;
    }

    u16 generate() {
        if (s_numbers.empty()) {
            get_data();
        }
        assert(!s_numbers.empty());
        // subsample by 4 (50 ms -> 200 ms sampling period)
        auto index = counter++;
        if (index >= s_numbers.size())
            counter = 0;
        return s_numbers[index % s_numbers.size()];
    }
}
using namespace record;

#else

#include "common/clock.h"
#include <cstdlib>
namespace synthetic {
    i8 inoise(double x) {
        const auto b1 = double(1) / 3;
        const auto b2 = 2 * b1;
        if (x < b1)
            return -1;
        if (x < b2)
            return 0;
        return 1;
    }
    i8 noise() {
        const double n = double(std::rand()) / RAND_MAX; // [0,1]
        return inoise(n);
    }

    constexpr u16 c = 5000;
    constexpr u64 T = 1000 * 3.6 * 1e6 / (75 * c);
    constexpr u64 T1 = 5 * T / 100;
    constexpr u16 m = 32;
    constexpr u16 M = 38;

    u16 generate() {
        auto t = Clock::millis_since_start();
        assert(T1 >= 1);
        const auto n = noise();
        u16 ret = (t % T) <= T1 ? M + n : m + n;
        assert((m - 1) <= ret && ret <= (M + 1));
        //DBG("t=%d T=%d t|T=%d T1=%d ret=%d\n",int(t),int(T),int(t%T),int(T1),ret);
        return ret;
    }
}
using namespace synthetic;
#endif

u16 analog::read() {
    return generate();
}
