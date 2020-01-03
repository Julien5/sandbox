#include "application.h"

int main(int argc, char ** argv) {
  application::setup();
  while(1)
    application::loop();
  return 0;
}

