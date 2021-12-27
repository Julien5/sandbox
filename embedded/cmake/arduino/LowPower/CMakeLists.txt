add_library(LowPower STATIC)
target_sources(LowPower PRIVATE
  /opt/arduino/libraries/LowPower/LowPower.cpp
  )
target_include_directories(LowPower PUBLIC /opt/arduino/libraries/LowPower)
