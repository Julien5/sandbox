#include "baseline.h"
#include "minesweeper.h"

#include <cassert>
#include <algorithm>
#include <cstring>
#include <random>
#include <memory>
#include <iostream>

namespace {
    const int8_t BOMB = 10;
    const int8_t EMPTY = 0;

    void count_mines_at_index(Minesweeper &M, const size &idx) {
        auto &grid(M.grid);
        const auto &X = M.X;
        const auto &Y = M.Y;
        assert(X == Y);
        point p = _2d(idx, X, Y);
        assert(0 <= p.x && p.x < X);
        assert(0 <= p.y && p.y < Y);
        for (int dx = -1; dx < 2; ++dx) {
            isize nx = isize(p.x) + dx;
            if (nx < 0 || nx >= X)
                continue;
            for (int dy = -1; dy < 2; ++dy) {
                if (dx == 0 && dy == 0)
                    continue;
                isize ny = isize(p.y) + dy;
                if (ny < 0 || ny >= Y)
                    continue;
                size nk = _1d(nx, ny, X, Y);
                assert(0 <= nk && nk <= (X * Y));
                if (grid[nk] != BOMB)
                    grid[nk]++;
            }
        }
    }

    std::vector<size> create_bombs(const size &X, const size &Y, const size &N) {
        std::vector<size> positions(X * Y, 0);
        size i = 0;
        for (size x = 0; x != X; ++x) {
            for (size y = 0; y != Y; ++y) {
                positions[i] = i;
                i++;
            }
        }
        return FisherYatesShuffle(positions, N, X, Y);
    }

    Minesweeper create(const size &X, const size &Y, const size &N) {
        Minesweeper ret;
        ret.bombs = create_bombs(X, Y, N);
        // for (auto &b : ret.bombs) {
        //    printf("b=%d\n", int(b));
        //}
        ret.grid = Grid(X * Y, EMPTY);
        ret.X = X;
        ret.Y = Y;
        size total = X * Y;
        for (const auto &idx : ret.bombs) {
            assert(idx < total);
            ret.grid[idx] = BOMB;
            count_mines_at_index(ret, idx);
        }
        return ret;
    }

    void print(const Minesweeper &M, const bool show_counts, const bool quiet) {
        const auto &grid(M.grid);
        const auto X = M.X;
        const auto Y = M.Y;
        char lookup[16] = {0};
        lookup[EMPTY] = ' ';
        lookup[BOMB] = '*';
        for (size i = 1; i < 9; ++i) {
            assert(i != EMPTY);
            assert(i != BOMB);
            lookup[i] = show_counts ? '0' + i : lookup[EMPTY];
        }
        std::vector<int8_t> output(4 * X + 2, ' ');
        output[0] = '|';
        for (size j = 0; j < X; ++j) {
            output[4 * j + 4] = '|';
        }
        output[4 * X + 1] = '\n';
        for (size iy = 0; iy < Y; ++iy) {
            for (size ix = 0; ix < X; ++ix) {
                const auto k = _1d(ix, iy, X, Y);
                const auto g = grid[k];
                output[4 * ix + 2] = lookup[g];
            }
            if (!quiet)
                fwrite(output.data(), sizeof(char), 4 * X + 2, stdout);
        }
    }
}

int baseline::run(size X, size Y, size N, bool quiet) {
    log("make bomb and counts");
    Minesweeper M = create(X, Y, N);
    log("print grid");
    print(M, false, quiet);
    log("print counts");
    print(M, true, quiet);
    log("done");
    return EXIT_SUCCESS;
}
