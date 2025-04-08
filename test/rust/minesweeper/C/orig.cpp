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
#include <cassert>

#include "minesweeper.h"
#include "orig.h"

namespace {
    static int8_t *g_grid;
    typedef size_t size;

    inline void create_grid(int8_t *grid, int X, int Y, grid_index *mine_grid_index,
                            int N) {
        int total = X * Y;
        std::memset(grid, '.', total);

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
            grid[idx] = 'M';
        }
    }

#define INC_COUNT(idx) grid[(idx)] += 0b1100 >> (grid[(idx)] >> 4)

    inline void count_mines(int8_t *grid, int X, int Y, grid_index *mine_grid_index,
                            int N) {
        int total = X * Y;
        //#pragma omp parallel for simd
        for (int n = 0; n < N; ++n) {
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

    int run_orig(size X, size Y, size N, bool quiet) {
        int total = X * Y;
        if (N > total)
            return -1;

        g_grid = new int8_t[total];
        grid_index *mine_grid_index = new grid_index[N];

        if (!g_grid || !mine_grid_index)
            return -1;

        create_grid(g_grid, X, Y, mine_grid_index, N);
        for (int i = 0; i < Y; ++i) {
            fwrite(&g_grid[i * X], sizeof(*g_grid), X, stdout);
            putc('\n', stdout);
        }
        count_mines(g_grid, X, Y, mine_grid_index, N);

        for (int i = 0; i < Y; ++i) {
            fwrite(&g_grid[i * X], sizeof(*g_grid), X, stdout);
            putc('\n', stdout);
        }

        delete[] g_grid;
        delete[] mine_grid_index;
        return EXIT_SUCCESS;
    }
}

int orig::run(size X, size Y, size N, bool quiet) {
    return ::run_orig(X, Y, N, quiet);
}
