#include <iostream>
#include "parse.h"

int main(int argc, char ** argv) {
  std::cout << "test\n";
  int exitcode=parse::test();
  std::cout << "exit code:" << exitcode << std::endl;
  if (exitcode!=0)
    std::cout << "bad.\n";
  else
    std::cout << "good.\n";
  return exitcode;
}
