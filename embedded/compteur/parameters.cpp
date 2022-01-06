#include "parameters.h"
#include "common/debug.h"

static std::vector<std::string> arguments;

void parameters::set(int argc, char **argv) {
    arguments = std::vector<std::string>(argv + 1, argv + argc);
}

std::vector<std::string> parameters::get() {
    return arguments;
}
