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

    void count_mines_at_index(Minesweeper &M, size idx) {
        Grid &grid(M.grid);
        const auto X = M.X;
        const auto Y = M.Y;
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

    std::vector<size> create_bombs(const size &X, const size &Y, const size &N) {
        std::vector<size> positions(X * Y, 0);
        size i = 0;
        for (size x = 0; x != X; ++x) {
            for (size y = 0; y != Y; ++y) {
                positions[i++] = y * X + x;
            }
        }
        return FisherYatesShuffle(positions, N, X, Y);
    }

    Minesweeper create(size X, size Y, size N) {
        Minesweeper M;
        M.X = X;
        M.Y = Y;
        M.bombs = create_bombs(X, Y, N);
        M.grid = Grid(X * Y, EMPTY);
        for (const auto &idx : M.bombs) {
            assert(idx < M.grid.size());
            M.grid[idx] = BOMB;
        }
        return M;
    }

    void count_mines(Minesweeper &M) {
        for (const auto &idx : M.bombs) {
            assert(M.grid[idx] = BOMB);
            count_mines_at_index(M, idx);
        }
    }

    void print(const Minesweeper &M, bool quiet) {
        if (quiet)
            return;
        const auto &grid(M.grid);
        const auto X = M.X;
        const auto Y = M.Y;
        auto ptr = grid.data();
        for (size i = 0; i < Y; ++i) {
            fwrite(ptr + (i * X), sizeof(Grid::value_type), X, stdout);
            putc('\n', stdout);
        }
    }
}

int fast::run(size X, size Y, size N, bool quiet) {
    log("make grid");
    auto minesweeper = create(X, Y, N);
    log("print bombs");
    print(minesweeper, quiet);
    log("count mines");
    count_mines(minesweeper);
    log("print counts");
    print(minesweeper, quiet);
    log("done");
    return EXIT_SUCCESS;
}
