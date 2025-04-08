#include "minesweeper.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <random>

void log(const std::string &msg) {
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

size _1d(size x, size y, size X, size Y) {
    return y * X + x;
}

point _2d(size k, size X, size Y) {
    point p;
    p.x = k % X;
    p.y = k / X;
    return p;
}
// Fisher–Yates_shuffle
void FisherYatesShuffle(std::vector<size> positions, grid_index *ret, size count, size X, size Y) {
    const auto max_size = X * Y;
    std::default_random_engine gen{static_cast<long unsigned int>(0)};
    // std::random_device rd;
    // std::minstd_rand0 gen(rd());
    for (size i = 0; i != count; ++i) {
        auto end = max_size - i - 1;
        std::uniform_int_distribution<> dis(0, end);
        size j = dis(gen); // rand() % end; // dis(gen);
        // todo: use std::swap
        auto tmp = positions[j];
        positions[j] = positions[end];
        positions[end] = tmp;
    }
    for (size i = 0; i != count; ++i) {
        ret[i] = positions[max_size - i - 1];
    }
}
