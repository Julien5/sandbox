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
typedef size_t grid_index;

// Fisher–Yates_shuffle
void FisherYatesShuffle(grid_index *arr, size count, size max_size,
                        std::minstd_rand0 &gen) {
    size *positions = new size[max_size];
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
    bool quiet = false;
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
        quiet = arguments[0].find("verbose") == std::string::npos;
        X = readsize(arguments[1]);
        Y = X;
        N = readsize(arguments[2]);
    }
    printf("X: %d, Y: %d, N: %d\n", X, Y, N);
}

inline void create_grid(int8_t *grid, size X, size Y, grid_index *mine_grid_index,
                        size N) {
    size total = X * Y;
    std::memset(grid, '.', total);

    std::random_device rd;
    // std::mt19937 g(rd());
    std::minstd_rand0 g(rd());
    FisherYatesShuffle(mine_grid_index, N, total, g);
    for (size n = 0; n < N; ++n) {
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

inline void count_mines(int8_t *grid, size X, size Y, grid_index *mine_grid_index,
                        size N) {
    int total = X * Y;
    //#pragma omp parallel for simd
    for (size n = 0; n < N; ++n) {
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

void print_grid_fast(int8_t *grid) {
    printf("print counts\n");
    for (size i = 0; i < Y; ++i) {
        if (!quiet) {
            fwrite(&g_grid[i * X], sizeof(*g_grid), X, stdout);
            putc('\n', stdout);
        }
    }
}

void print_grid_slow(int8_t *grid) {
    printf("print counts\n");
    char lookup[32] = {' '};
    for (size i = 0; i < Y; ++i) {
        // fwrite(&g_grid[i * X], sizeof(*g_grid), X, stdout);
        int8_t *output = new int8_t[4 * X + 2];
        for (size j = 0; j < 4 * X + 2; ++j) {
            output[j] = ' ';
        }
        output[0] = '|';
        output[4 * X + 1] = '\n';
        for (size j = 0; j < X; ++j) {
            output[4 * j + 1] = ' ';
            output[4 * j + 2] = grid[i * X + j];
            output[4 * j + 3] = ' ';
            output[4 * j + 4] = '|';
        }
        if (!quiet)
            fwrite(output, sizeof(char), 4 * X + 2, stdout);
    }
}

void print_grid(int8_t *grid) {
    print_grid_slow(grid);
}

int run(const std::vector<std::string> &arguments) {
    init(arguments);
    size total = X * Y;
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
    print_grid(g_grid);
#endif
    printf("count mines\n");
    count_mines(g_grid, X, Y, mine_grid_index, N);

#ifdef PRINT
    printf("print counts\n");
    print_grid(g_grid);
#endif

    delete[] g_grid;
    delete[] mine_grid_index;
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    return run(std::vector<std::string>(argv + 1, argv + argc));
}
