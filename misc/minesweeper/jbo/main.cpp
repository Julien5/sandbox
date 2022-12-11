#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <map>
#include <math.h>
#include <set>
#include <stdlib.h>
#include <cstdlib>
#include <vector>
#include <time.h>

#define TRACE()                     \
    do {                            \
        printf("[%d]\n", __LINE__); \
    } while (0)

#define DBG(...)                   \
    do {                           \
        printf("[%d] ", __LINE__); \
        printf(__VA_ARGS__);       \
        printf("\n");              \
        fflush(stdout);            \
    } while (0)

typedef size_t size;

namespace global {
    // 40000 40000 1400000000
    size XSIZE = 1000;
    size YSIZE = 1000;
    // size NBOMBS = 1400000000;
    size NBOMBS = 1400;
} // namespace global

namespace {
    size readsize(const std::string &s) {
        char *p_end;
        return std::strtol(s.c_str(), &p_end, 10);
    }
}

void init(const std::vector<std::string> &arguments) {
    // TODO: https://stackoverflow.com/questions/31089952/using-rand-rand-max-1
    // init random number generator.
    srand(time(NULL));

    using namespace global;
    if (!arguments.empty()) {
        XSIZE = readsize(arguments[0]);
        YSIZE = readsize(arguments[0]);
        NBOMBS = readsize(arguments[0]) / 10;
        if (arguments.size() > 1) {
            YSIZE = readsize(arguments[1]);
        }
        if (arguments.size() > 2) {
            XSIZE = readsize(arguments[0]);
            YSIZE = readsize(arguments[1]);
            NBOMBS = readsize(arguments[2]);
        }
    }
    DBG("%lu %lu %lu", XSIZE, YSIZE, NBOMBS);
}

class Position {
    size m_k = 0;

  public:
    Position(size k) { m_k = k; }
    Position(size x, size y) : m_k(y * global::XSIZE + x) {}
    size y() const { return std::floor(k() / global::XSIZE); }
    size x() const { return m_k - y() * global::XSIZE; }

    size k() const { return m_k; }
    Position &operator++(int) {
        m_k++;
        return *this;
    }
    bool operator<(const Position &other) const { return k() < other.k(); }
    static Position max() { return Position(global::XSIZE, global::YSIZE - 1); }
};

class Neighbors {
    typedef std::map<Position, int> Map;

    Map m_map;

  public:
    void merge(const Neighbors &other) {
        for (auto b : other.m_map) {
            const auto &pos = b.first;
            const auto &count = b.second;
            auto it = m_map.find(pos);
            auto other_is_bomb = count == 0;
            if (it != m_map.end()) {
                it->second += count;
            } else {
                m_map[pos] = count;
            }
        }
    }
    void indicate_bombs(const std::vector<Position> bombs) {
        for (auto bomb : bombs) {
            m_map[bomb] = 0;
        }
    }
    void set(const Position &pos, const int &count) { m_map[pos] = count; }
    void print() const {
        printf("neighbors()\n");
        //#define JUMP
#ifdef JUMP
        for (auto it = m_map.begin(); it != m_map.end(); ++it) {
            const Position pos = it->first;
            const int count = it->second;
            if (count != 0) {
                printf("%d", count);
            } else {
                printf("*");
            }
        }
#else
        using namespace global;
        for (size x = 0; x < XSIZE; ++x) {
            for (size y = 0; y < YSIZE; ++y) {
                const Position pos(x, y);
                auto it = m_map.find(pos);
                if (it != m_map.end()) {
                    if (it->second != 0) {
                        printf("%d", it->second);
                    } else {
                        printf("*");
                    }
                } else {
                    printf(".");
                }
            }
            printf("\n");
        }
#endif
    }
};

class Bomb {
    Position m_pos;

  public:
    Bomb(Position pos) : m_pos(pos){};
    Position position() const { return m_pos; }
    Bomb &operator++(int) {
        m_pos++;
        return *this;
    }
    bool operator<(const Bomb &other) const {
        return position() < other.position();
    }
    bool operator==(const Bomb &other) const {
        const Bomb &A(*this);
        const Bomb &B(other);
        return !(A < B) && !(B < A);
    }
    Neighbors neighbors() {
        Neighbors ret;
        const auto x = position().x();
        const auto y = position().y();
        for (int k = -1; k <= 1; ++k) {
            for (int l = -1; l <= 1; ++l) {
                bool xok = 0 <= (int(x) + k) && (int(x) + k) < global::XSIZE;
                bool yok = 0 <= (int(y) + l) && (int(y) + l) < global::YSIZE;
                if (!xok)
                    continue;
                if (!yok)
                    continue;
                ret.set(Position(x + k, y + l), 1);
            }
        }
        return ret;
    }
};

size random(size max) {
    // return 0;
    int raw = rand();
    // < max (strict inferior)
    int r = std::floor(max * float(raw) / RAND_MAX);
    return r;
}

Bomb generateRandomBomb(const std::set<Bomb> &excluded) {
    auto max = Position::max().k() - excluded.size();
    Bomb r = Bomb(random(max));
    auto it = excluded.find(r);
    if (it == excluded.end())
        return r;
    size n = std::distance(excluded.begin(), it);
    return Bomb(Position(max + n));
}

class Board {
    std::set<Bomb> m_bombs;

  public:
    Board(const size nbombs) {
        assert(m_bombs.empty());
        while (m_bombs.size() != nbombs) {
            m_bombs.insert(generateRandomBomb(m_bombs));
        }
        assert(m_bombs.size() == nbombs);
    }

    bool bomb_at(size x, size y) {
        return m_bombs.find(Bomb(Position(x, y))) != m_bombs.end();
    }

    std::set<Bomb> bombs() const { return m_bombs; }
    std::vector<Position> bombpositions() const {
        std::vector<Position> ret;
        ret.reserve(m_bombs.size());
        for (auto b : m_bombs) {
            ret.push_back(b.position());
        }
        return ret;
    }

    void print() {
        using namespace global;
        //for (auto b : m_bombs)
        //   printf("bomb at: %lu %lu\n", b.position().x(), b.position().y());
        for (size x = 0; x < XSIZE; ++x) {
            for (size y = 0; y < YSIZE; ++y) {
                if (bomb_at(x, y))
                    printf("%s", "*");
                else
                    printf("%s", ".");
            }
            printf("\n");
        }
    }
};

int run(const std::vector<std::string> &arguments) {
    init(arguments);
    Board b(global::NBOMBS);
    TRACE();
    b.print();
    Neighbors N;
    for (auto bomb : b.bombs()) {
        N.merge(bomb.neighbors());
    }
    N.indicate_bombs(b.bombpositions());
    TRACE();
    N.print();
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    return run(std::vector<std::string>(argv + 1, argv + argc));
}
