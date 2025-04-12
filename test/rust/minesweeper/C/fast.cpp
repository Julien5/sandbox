#include "fast.h"
#include "minesweeper.h"
#include <cassert>
#include <cstring>
#include <random>
#include <iostream>

namespace {
    const int8_t BOMB = 'M';
    const int8_t EMPTY = '.';

#define INC_COUNT(idx) grid[(idx)] += 0b1100 >> (grid[(idx)] >> 4)

    inline void count_mines_at_index(Grid &grid, size X, size Y, size idx) {
        point p = _2d(idx, X, Y);
        // std::cerr << "p.x=" << int(p.x) << " p.y=" << int(p.y) << std::endl;
        assert(0 <= p.x && p.x < X);
        assert(0 <= p.y && p.y < Y);
        for (int dx = -1; dx < 2; ++dx) {
            for (int dy = -1; dy < 2; ++dy) {
                if (dx == 0 && dy == 0)
                    continue;
                int64_t nx = int64_t(p.x) + dx;
                int64_t ny = int64_t(p.y) + dy;
                if (nx < 0 || nx >= X)
                    continue;
                if (ny < 0 || ny >= Y)
                    continue;
                int64_t nk = ny * X + nx;
                // std::cerr << "p.x=" << int(p.x) << " p.y=" << int(p.y)
                //          << " ny=" << ny << " nx=" << nx << " nk=" << nk << std::endl;
                //  out of bounds
                assert(0 <= nk && nk <= (X * Y));
                // bomb
                INC_COUNT(nk);
            }
        }
    }

    inline Grid create_grid(size X, size Y, Bombs &bombs, size N) {
        const size total = X * Y;
        auto ret = Grid(total, EMPTY);
        std::vector<size> positions(X * Y, 0);
        size i = 0;
        for (size x = 0; x != X; ++x) {
            for (size y = 0; y != Y; ++y) {
                positions[i++] = y * X + x;
            }
        }
        bombs = FisherYatesShuffle(positions, N, X, Y);
        for (size n = 0; n < N; ++n) {
            const auto idx = bombs[n];
            assert(idx < total);
            ret[idx] = BOMB;
        }
        return ret;
    }

    inline void count_mines(Grid &grid, size X, size Y, const Bombs &bombs, size N) {
        for (size n = 0; n < N; ++n) {
            const auto idx = bombs[n];
            count_mines_at_index(grid, X, Y, idx);
        }
    }

    void print_grid(const Grid &grid, size X, size Y, bool quiet) {
        if (quiet)
            return;
        auto ptr = grid.data();
        for (size i = 0; i < Y; ++i) {
            fwrite(ptr + (i * X), sizeof(Grid::value_type), X, stdout);
            putc('\n', stdout);
        }
    }
}

int fast::run(size X, size Y, size N, bool quiet) {
    log("make grid");
    Bombs bombs(N);
    Grid grid = create_grid(X, Y, bombs, N);

    log("print bombs");
    print_grid(grid, X, Y, quiet);
    log("count mines");
    count_mines(grid, X, Y, bombs, N);
    log("print counts");
    print_grid(grid, X, Y, quiet);
    log("done");
    return EXIT_SUCCESS;
}
