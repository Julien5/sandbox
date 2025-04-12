#pragma once

#include <cstddef>
#include <string>
#include <vector>

typedef std::size_t size;

using Grid = std::vector<int8_t>;
using Bombs = std::vector<std::size_t>;

struct Minesweeper {
    Grid grid;
    Bombs bombs;
    size X;
    size Y;
};

void log(const std::string &msg);
struct point {
    size x = 0;
    size y = 0;
};

inline size _1d(size x, size y, size X, size Y) {
    return y * X + x;
}

inline point _2d(size k, size X, size Y) {
    point p;
    p.x = k % X;
    p.y = k / X;
    return p;
}

std::vector<size> FisherYatesShuffle(std::vector<size> &positions, const size &count, const size &X, const size &Y);
