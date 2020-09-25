#include "application.h"

#include <chrono>
#include <thread>

int main(int, char **) {
    application::setup();
    while (1)
        application::loop();
    return 0;
}
