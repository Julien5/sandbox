#include <iostream>
#include "parse.h"
//#include "wifi.h"
#include "nstring.h"
#include "statistics.h"
#include "clock.h"
#include "tickscounter.h"
#include "ui.h"

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
  display(nstring::test());
  display(Clock::test());
  display(statistics::test());
  display(tickscounter::test());
  display(ui::test());
  return 0;
}
