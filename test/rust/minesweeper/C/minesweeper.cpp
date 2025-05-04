#include "minesweeper.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <random>

void log(const std::string &msg) {
    return;
    // get a precise timestamp as a string
    const auto now = std::chrono::system_clock::now();
    const auto nowAsTimeT = std::chrono::system_clock::to_time_t(now);
    const auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                           now.time_since_epoch()) %
                       1000;
    std::cerr
        << std::put_time(std::localtime(&nowAsTimeT), "%T")
        << '.' << std::setfill('0') << std::setw(3) << nowMs.count()
        << ": "
        << msg << std::endl;
}

struct Random {
    size xn = 0;
    size a = 1664525;
    size b = 1013904223;
    size modulus = std::pow(2, 32);
    std::default_random_engine gen;
    size range(const size &begin, const size &end) {
        if (std::getenv("TEST")) {
            auto d = end - begin;
            xn = (a * xn + b) % modulus;
            return (begin + xn) % d;
        }
        std::uniform_int_distribution<> dis(0, end);
        return dis(this->gen);
    }
};

// Fisher–Yates_shuffle
std::vector<size> FisherYatesShuffle(std::vector<size> &positions, const size &count, const size &X, const size &Y) {
    const auto max_size = X * Y;
    Random random;
    random.gen = std::default_random_engine{static_cast<long unsigned int>(0)};
    for (size i = 0; i != count; ++i) {
        const auto end = max_size - i;
        const size j = random.range(0, end);
        std::swap(positions[j], positions[end - 1]);
    }
    std::vector<size> ret(count);
    for (size i = 0; i != count; ++i) {
        ret[i] = positions[max_size - i - 1];
    }
    return ret;
}
