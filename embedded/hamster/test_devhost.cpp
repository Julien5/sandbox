#include "test_devhost.h"

#include <iostream>
#include "parse.h"
#include "eeprom.h"
#include "nstring.h"
#include "clock.h"
#include "tickscounter.h"
#include "ui.h"

#include <vector>
#include <algorithm>

#include "application.h"

int
display_exit_code(int exitcode)
{
  if (exitcode != 0)
    std::cout << "bad.\n";
  else
    std::cout << "good.\n";
  if (exitcode != 0)
    exit(exitcode);
  return exitcode;
}

int
test::run()
{
  std::cout << "test\n";
  display_exit_code(eeprom::test());
  display_exit_code(parse::test());
  display_exit_code(nstring::test());
  display_exit_code(Clock::test());
  display_exit_code(tickscounter::test());
  display_exit_code(ui::test());
  return 0;
}
