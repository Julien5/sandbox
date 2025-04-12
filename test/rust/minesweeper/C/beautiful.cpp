#include "beautiful.h"

#include "minesweeper.h"

#include <cassert>
#include <algorithm>
#include <cstring>
#include <random>
#include <memory>

namespace {
    const int8_t BOMB = 10;
    const int8_t EMPTY = 0;

    void count_mines_at_index(Minesweeper &M, size idx) {
        auto &grid(M.grid);
        const auto X = M.X;
        const auto Y = M.X;
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

    std::vector<size> create_bombs(const size &X, const size &Y, const size &N) {
        std::vector<size> positions(X * Y, 0);
        size i = 0;
        for (size x = 1; x != X + 1; ++x) {
            for (size y = 1; y != Y + 1; ++y) {
                positions[i++] = y * (X + 2) + x;
            }
        }
        return FisherYatesShuffle(positions, N, X, Y);
    }

    Minesweeper create(const size &X, const size &Y, const size &N) {
        Minesweeper ret;
        ret.bombs = create_bombs(X, Y, N);
        ret.grid = Grid((X + 2) * (Y + 2), EMPTY);
        ret.X = X;
        ret.Y = Y;
        size total = (X + 2) * (Y + 2);
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
        lookup[EMPTY] = '.';
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
        for (size i = 0; i < Y; ++i) {
            for (size j = 0; j < X; ++j) {
                const auto k = _1d(i + 1, j + 1, X + 2, Y + 2);
                const auto g = grid[k];
                output[4 * j + 2] = lookup[g];
            }
            if (!quiet)
                fwrite(output.data(), sizeof(char), 4 * X + 2, stdout);
        }
    }
}

int beautiful::run(size X, size Y, size N, bool quiet) {
    log("make bomb and counts");
    Minesweeper M = create(X, Y, N);
    log("print grid");
    print(M, false, quiet);
    log("print counts");
    print(M, true, quiet);
    log("done");
    return EXIT_SUCCESS;
}
