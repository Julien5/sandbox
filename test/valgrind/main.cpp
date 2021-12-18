#include <stdio.h>
#include <thread>
#include <chrono>

void spike() {
    int n[16 * 1024] = {0};
}

void mark() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    spike();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

int main() {
    mark();
    int n[256] = {0};
    mark();
    return 0;
}
