#include <iostream>
#include "core/core.h"

int main(int argc, char **argv) {
    std::cout << "hello world, core:" << core::foo() << std::endl;
    return 0;
}
