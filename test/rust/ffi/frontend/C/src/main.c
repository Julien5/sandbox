#include <stdio.h>
#include "backend.h"

int main() {
    printf("Hello, World!\n");
    uint64_t a = 2;
    uint64_t b = 3;
    uint64_t c = add(a, b);
    printf("a+b = %d\n", c);
    return 0;
}
