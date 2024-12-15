#include "application.h"

int main(int argc, char **argv) {
    application::setup();
    while (true) {
        application::loop();
    }
}
