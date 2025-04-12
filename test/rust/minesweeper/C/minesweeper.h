#pragma once

#include <cstddef>
#include <string>
#include <vector>

typedef std::size_t size;

using Grid = std::vector<int8_t>;
using Bombs = std::vector<std::size_t>;

void log(const std::string &msg);
struct point {
    size x = 0;
    size y = 0;
};

size _1d(size x, size y, size X, size Y);
point _2d(size k, size X, size Y);

std::vector<size> FisherYatesShuffle(std::vector<size> positions, size count, size X, size Y);
