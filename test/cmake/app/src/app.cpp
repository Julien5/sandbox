#include <iostream>
#include "core/core.h"
#include "lib/lib.h"

int main(int argc, char **argv) {
    std::cout << "hello world, core:" << core::foo() << " bar:" << lib::bar() << std::endl;
    return 0;
}
