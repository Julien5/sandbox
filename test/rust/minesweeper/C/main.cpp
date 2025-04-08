#include <iostream>
#include <vector>
#include "fast.h"
#include "beautiful.h"
#include "minesweeper.h"
#include "orig.h"
#include <cassert>

namespace global {
    size X = 0;
    size Y = 0;
    size N = 0;
    bool quiet = false;
    std::string algorithm = std::string();
} // namespace global

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
        global::algorithm = arguments[0];
        global::quiet = arguments[1].find("verbose") == std::string::npos;
        global::X = readsize(arguments[2]);
        global::Y = global::X;
        global::N = readsize(arguments[3]);
    }
    printf("algorithm: %s, X: %d, Y: %d, N: %d\n", global::algorithm.c_str(),
           global::X, global::Y, global::N);
}

int run(const std::vector<std::string> &arguments) {
    init(arguments);
    size total = global::X * global::Y;
    if (global::N > total) {
        assert(false);
        return -1;
    }
    if (global::algorithm == "fast")
        return fast::run(global::X, global::Y, global::N, global::quiet);
    else if (global::algorithm == "beautiful")
        return beautiful::run(global::X, global::Y, global::N, global::quiet);
    else if (global::algorithm == "orig")
        return orig::run(global::X, global::Y, global::N, global::quiet);
    else {
        std::cerr << "unknown:" << global::algorithm << std::endl;
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    return run(std::vector<std::string>(argv + 1, argv + argc));
}
