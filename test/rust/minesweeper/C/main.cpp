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

#include <chrono>
#include <iomanip>
#include <iostream>

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

static int8_t *g_grid;
typedef size_t size;
typedef size_t grid_index;
const int8_t BOMB = 10;
const int8_t EMPTY = 0;
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
    std::memset(grid, EMPTY, total);

    std::random_device rd;
    // std::mt19937 g(rd());
    std::minstd_rand0 g(rd());
    FisherYatesShuffle(mine_grid_index, N, total, g);
    for (size n = 0; n < N; ++n) {
        grid_index idx = mine_grid_index[n];
        grid[idx] = BOMB;
    }
}

#define INC_COUNT(idx)         \
    if (grid[(idx)] != BOMB) { \
        grid[(idx)]++;         \
    }

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
    for (size i = 0; i < Y; ++i) {
        if (!quiet) {
            fwrite(&g_grid[i * X], sizeof(*g_grid), X, stdout);
            putc('\n', stdout);
        }
    }
}

void print_grid_slow(int8_t *grid, const bool show_counts) {
    if (show_counts)
        log("print counts");
    else
        log("print grid");
    char lookup[16] = {13};
    lookup[EMPTY] = '.';
    lookup[BOMB] = '*';
    for (size i = 1; i < 9; ++i) {
        lookup[i] = show_counts ? '0' + i : lookup[EMPTY];
    }

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
            output[4 * j + 2] = lookup[grid[i * X + j]];
            output[4 * j + 3] = ' ';
            output[4 * j + 4] = '|';
        }
        if (!quiet)
            fwrite(output, sizeof(char), 4 * X + 2, stdout);
    }
}

void print_grid(int8_t *grid, bool show_counts) {
    print_grid_slow(grid, show_counts);
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

    log("make grid");
    create_grid(g_grid, X, Y, mine_grid_index, N);
    log("count mines");
    count_mines(g_grid, X, Y, mine_grid_index, N);
    print_grid(g_grid, false);
    print_grid(g_grid, true);

    delete[] g_grid;
    delete[] mine_grid_index;
    log("done");
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    return run(std::vector<std::string>(argv + 1, argv + argc));
}
