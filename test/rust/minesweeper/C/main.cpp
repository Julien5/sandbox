#include <algorithm> //random_shuffle
#include <array>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <cassert>

#include <chrono>
#include <iomanip>
#include <iostream>

// #define FAST_PRINT

static int8_t *g_grid = nullptr;

#ifndef FAST_PRINT
const int8_t BOMB = 10;
const int8_t EMPTY = 0;
#else
const int8_t BOMB = 'M';
const int8_t EMPTY = '0';
#endif

typedef size_t size;
typedef size_t grid_index;

namespace global {
    size X = 10;
    size Y = 10;
    size N = 10;
    bool quiet = false;
} // namespace global

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

// Fisher–Yates_shuffle
void FisherYatesShuffle(grid_index *arr, size count, size X, size Y,
                        std::minstd_rand0 &gen) {
    const auto max_size = X * Y;
    size *positions = new size[max_size];
    size i = 0;
    for (size x = 1; x != X + 1; ++x) {
        for (size y = 1; y != Y + 1; ++y) {
            positions[i++] = y * (X + 2) + x;
        }
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
        global::quiet = arguments[0].find("verbose") == std::string::npos;
        global::X = readsize(arguments[1]);
        global::Y = global::X;
        global::N = readsize(arguments[2]);
    }
    printf("X: %d, Y: %d, N: %d\n", global::X, global::Y, global::N);
}

size _1d(size x, size y, size X, size Y) {
    return y * X + x;
}

struct point {
    size x = 0;
    size y = 0;
};

point _2d(int k, size X, size Y) {
    point p;
    p.x = k % X;
    p.y = k / X;
    return p;
}

inline void count_mines_at_index(int8_t *grid, size idx) {
    using namespace global;
    size i = idx % (X + 2);
    // std::cerr << "X=" << X << " Y=" << Y << " idx = " << idx << " i = " << i << std::endl;
    assert(i > 0 && i < (X + 2 - 1));
    size j = idx / (X + 2);
    assert(j > 0 && j < (Y + 2 - 1));
    point p = _2d(idx, X + 2, Y + 2);
    assert(0 < p.x && p.x < X + 1);
    assert(0 < p.y && p.y < Y + 1);
    for (int dx = -1; dx < 2; ++dx) {
        for (int dy = -1; dy < 2; ++dy) {
            if (dx == 0 && dy == 0)
                continue;
            size nx = p.x + dx;
            size ny = p.y + dy;
            size nk = _1d(nx, ny, X + 2, Y + 2);
            if (grid[nk] != BOMB)
                grid[nk]++;
        }
    }
}

inline void create_grid(int8_t *grid, grid_index *mine_grid_index) {
    size total = (global::X + 2) * (global::Y + 2);
    std::memset(grid, EMPTY, total);

    std::random_device rd;
    // std::mt19937 g(rd());
    std::minstd_rand0 g(rd());
    FisherYatesShuffle(mine_grid_index, global::N, global::X, global::Y, g);
    for (size n = 0; n < global::N; ++n) {
        grid_index idx = mine_grid_index[n];
        assert(idx < total);
        grid[idx] = BOMB;
#ifndef FAST_PRINT
        count_mines_at_index(grid, idx);
#endif
    }
}

inline void count_mines(int8_t *grid, grid_index *mine_grid_index) {
    for (size n = 0; n < global::N; ++n) {
        grid_index idx = mine_grid_index[n];
        count_mines_at_index(grid, idx);
    }
}

void print_grid_fast(int8_t *grid, bool _show_count) {
    using namespace global;
    for (size i = 0; i < Y; ++i) {
        if (!quiet) {
            fwrite(&g_grid[(i + 1) * (X + 2)], sizeof(*g_grid), X + 2, stdout);
            putc('\n', stdout);
        }
    }
}
#include <cassert>
void print_grid_slow(int8_t *grid, const bool show_counts) {
    using namespace global;
    char lookup[16] = {0};
    lookup[EMPTY] = '.';
    lookup[BOMB] = '*';
    for (size i = 1; i < 9; ++i) {
        assert(i != EMPTY);
        assert(i != BOMB);
        lookup[i] = show_counts ? '0' + i : lookup[EMPTY];
        // lookup['0' + i] = show_counts ? '0' + i : lookup[EMPTY];
    }
    int8_t *output = new int8_t[4 * X + 2];
    for (size i = 0; i < Y; ++i) {
        for (size j = 0; j < 4 * X + 2; ++j) {
            output[j] = ' ';
        }
        output[0] = '|';
        output[4 * X + 1] = '\n';
        for (size j = 0; j < X; ++j) {
            output[4 * j + 1] = ' ';
            const auto k = _1d(i + 1, j + 1, X + 2, Y + 2);
            const auto g = grid[k];
            output[4 * j + 2] = lookup[g];
            output[4 * j + 3] = ' ';
            output[4 * j + 4] = '|';
        }
        if (!quiet)
            fwrite(output, sizeof(char), 4 * X + 2, stdout);
    }
    delete output;
}

void print_grid(int8_t *grid, bool show_counts) {
    log("print grid");
#ifdef FAST_PRINT
    print_grid_fast(grid, show_counts);
#else
    print_grid_slow(grid, show_counts);
#endif
}

int run(const std::vector<std::string> &arguments) {
    init(arguments);
    size total = global::X * global::Y;
    if (global::N > total)
        return -1;
    log("make grid");
    g_grid = new int8_t[(global::X + 2) * (global::Y + 2)];
    grid_index *mine_grid_index = new grid_index[global::N];
    if (!g_grid || !mine_grid_index)
        return -1;
    create_grid(g_grid, mine_grid_index);

#ifdef FAST_PRINT
    print_grid(g_grid, false);
    log("count mines");
    count_mines(g_grid, mine_grid_index);
#else
    print_grid(g_grid, false);
#endif
    print_grid(g_grid, true);

    delete[] g_grid;
    delete[] mine_grid_index;
    log("done");
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    return run(std::vector<std::string>(argv + 1, argv + argc));
}
