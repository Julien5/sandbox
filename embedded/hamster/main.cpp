#include "application.h"

extern "C" {
  void setup()
  {
    application::setup();
  }

  void loop() {
    application::loop();
  }
}

#ifdef DEVHOST
#include <vector>
#include <string>
#include <algorithm>
std::vector<std::string> args(int argc, char ** argv) {
  return std::vector<std::string>(argv,argv+argc);
}
#include "test_devhost.h"
int main(int argc, char ** argv) {
  auto A=args(argc,argv);
  if (std::find(A.begin(),A.end(),std::string("test"))!=A.end()) {
    return test::run();
  }
  
  setup();
  while(true) {
    loop();
  }
  return 0;
}
#endif

#ifdef ESP8266
extern "C" {
  void user_init(void)
  {
  }
}
#endif
