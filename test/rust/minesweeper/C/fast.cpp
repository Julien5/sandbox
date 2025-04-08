#include "fast.h"
#include "minesweeper.h"
#include <cassert>
#include <cstring>
#include <random>
#include <iostream>

namespace {
    static int8_t *g_grid = nullptr;
    const int8_t BOMB = 'M';
    const int8_t EMPTY = '.';

#define INC_COUNT(idx) grid[(idx)] += 0b1100 >> (grid[(idx)] >> 4)

    inline void create_grid(int8_t *grid, size X, size Y, grid_index *mine_grid_index, size N) {
        const size total = X * Y;
        std::memset(grid, EMPTY, total);

        std::vector<size> positions(X * Y, 0);
        size i = 0;
        for (size x = 0; x != X; ++x) {
            for (size y = 0; y != Y; ++y) {
                positions[i++] = y * X + x;
            }
        }
        FisherYatesShuffle(positions, mine_grid_index, N, X, Y);
        for (size n = 0; n < N; ++n) {
            grid_index idx = mine_grid_index[n];
            assert(idx < total);
            grid[idx] = BOMB;
        }
    }

    inline void count_mines(int8_t *grid, size X, size Y, grid_index *mine_grid_index, size N) {
        const auto total = X * Y;
        for (size n = 0; n < N; ++n) {
            grid_index idx = mine_grid_index[n];
            int i = idx % X;

            grid[idx] = 'M';

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

    void print_grid(int8_t *grid, size X, size Y, bool quiet) {
        if (quiet)
            return;
        for (size i = 0; i < Y; ++i) {
            fwrite(&g_grid[i * X], sizeof(*g_grid), X, stdout);
            putc('\n', stdout);
        }
    }
}

int fast::run(size X, size Y, size N, bool quiet) {
    log("make grid");
    g_grid = new int8_t[X * Y];
    grid_index *mine_grid_index = new grid_index[N];
    if (!g_grid || !mine_grid_index)
        return -1;
    create_grid(g_grid, X, Y, mine_grid_index, N);

    print_grid(g_grid, X, Y, quiet);
    log("count mines");
    count_mines(g_grid, X, Y, mine_grid_index, N);
    print_grid(g_grid, X, Y, quiet);

    delete[] g_grid;
    delete[] mine_grid_index;
    log("done");
    return EXIT_SUCCESS;
}
