#include <iostream>
#include "parse.h"
#include "wifi.h"

int display(int exitcode) {
  if (exitcode!=0)
    std::cout << "bad.\n";
  else
    std::cout << "good.\n";
  if (exitcode!=0)
    exit(exitcode);
  return exitcode;
}

int main(int argc, char ** argv) {
  std::cout << "test\n";
  display(parse::test());
  display(wifi::test());
  return 0;
}
