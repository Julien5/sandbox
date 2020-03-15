#pragma once

namespace application {
  void setup();
#ifdef DEVHOST
  void loop_arduino();
#endif
  void loop_serial();
  void loop_wifi();
}
