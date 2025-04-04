#include <algorithm> //random_shuffle
#include <array>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <time.h>
#include <vector>

#define PRINT

static int8_t *g_grid;
typedef size_t size;
typedef uint32_t grid_index;

// Fisher–Yates_shuffle
void FisherYatesShuffle(grid_index *arr, size count, size max_size,
                        std::minstd_rand0 &gen) {
    int32_t *positions = new int32_t[max_size];
    for (size i = 0; i != max_size; ++i) {
        positions[i] = i;
    }
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
        arr[i] = positions[max_size - i - 1];
    }
    delete positions;
}

namespace global {
    size X = 10;
    size Y = 10;
    size N = 10;
} // namespace global

using namespace global;
namespace {
    size readsize(const std::string &s) {
        char *p_end;
        return std::strtol(s.c_str(), &p_end, 10);
    }
} // namespace

void init(const std::vector<std::string> &arguments) {
    // TODO: https://stackoverflow.com/questions/31089952/using-rand-rand-max-1
    // init random number generator.
    srand(time(NULL));

    if (!arguments.empty()) {
        X = readsize(arguments[0]);
        Y = readsize(arguments[0]);
        N = readsize(arguments[0]) / 10;
        if (arguments.size() > 1) {
            Y = readsize(arguments[1]);
        }
        if (arguments.size() > 2) {
            X = readsize(arguments[0]);
            Y = readsize(arguments[1]);
            N = readsize(arguments[2]);
        }
    }
    // printf("X: %d, Y: %d, N: %d\n", X, Y, N);
}

inline void create_grid(int8_t *grid, int X, int Y, grid_index *mine_grid_index,
                        int N) {
    int total = X * Y;
    std::memset(grid, '.', total);

    std::random_device rd;
    // std::mt19937 g(rd());
    std::minstd_rand0 g(rd());
    FisherYatesShuffle(mine_grid_index, N, total, g);
    for (int n = 0; n < N; ++n) {
        grid_index idx = mine_grid_index[n];
        grid[idx] = 'M';
    }
}

#define INC_COUNT(idx) grid[(idx)] += 0b1100 >> (grid[(idx)] >> 4)
/*
#define INC_COUNT(idx) \
        if (grid[(idx)] == '.') { \
                grid[(idx)] = '1'; \
        } else if (grid[(idx)] != 'M') { \
                grid[(idx)]++; \
        }
*/

inline void count_mines(int8_t *grid, int X, int Y, grid_index *mine_grid_index,
                        int N) {
    int total = X * Y;
    //#pragma omp parallel for simd
    for (int n = 0; n < N; ++n) {
        grid_index idx = mine_grid_index[n];
        int i = idx % X;

        if (idx > X) [[likely]] {
            if (i > 0) [[likely]] {
                INC_COUNT(idx - 1 - X);
            }
            INC_COUNT(idx - X);
            if (i + 1 < X) [[likely]] {
                INC_COUNT(idx + 1 - X);
            }
        }

        if (i > 0) [[likely]] {
            INC_COUNT(idx - 1);
        }
        if (i + 1 < X) [[likely]] {
            INC_COUNT(idx + 1);
        }

        if (idx < total - X) [[likely]] {
            if (i > 0) [[likely]] {
                INC_COUNT(idx - 1 + X);
            }
            INC_COUNT(idx + X);
            if (i + 1 < X) [[likely]] {
                INC_COUNT(idx + 1 + X);
            }
        }
    }
}

int run(const std::vector<std::string> &arguments) {
    init(arguments);
    int total = X * Y;
    if (N > total)
        return -1;

    g_grid = new int8_t[total];
    grid_index *mine_grid_index = new grid_index[N];

    if (!g_grid || !mine_grid_index)
        return -1;

    printf("make grid\n");
    create_grid(g_grid, X, Y, mine_grid_index, N);
#ifdef PRINT
    printf("print grid\n");
    for (int i = 0; i < Y; ++i) {
        fwrite(&g_grid[i * X], sizeof(*g_grid), X, stdout);
        putc('\n', stdout);
    }
#endif
    printf("count mines\n");
    count_mines(g_grid, X, Y, mine_grid_index, N);

#ifdef PRINT
    printf("print counts\n");
    for (int i = 0; i < Y; ++i) {
        fwrite(&g_grid[i * X], sizeof(*g_grid), X, stdout);
        putc('\n', stdout);
    }
#endif

    delete[] g_grid;
    delete[] mine_grid_index;
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    return run(std::vector<std::string>(argv + 1, argv + argc));
}
