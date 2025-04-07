#pragma once

#include <cstddef>
#include <string>

typedef size_t size;
typedef size_t grid_index;
void log(const std::string &msg);
struct point {
    size x = 0;
    size y = 0;
};
size _1d(size x, size y, size X, size Y);
point _2d(int k, size X, size Y);
void FisherYatesShuffle(grid_index *arr, size count, size X, size Y);
