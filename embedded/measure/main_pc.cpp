#include "message.h"
#include "application.h"

#include <chrono>
#include <thread>

void wait(int n = 500) {
    std::this_thread::sleep_for(std::chrono::milliseconds(n));
}

int main(int, char **) {
    application::setup();
    while (1)
        application::loop();
    return 0;
}
